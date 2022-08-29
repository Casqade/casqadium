#include <cqde/types/EntityManager.hpp>

#include <cqde/util/logger.hpp>

#include <cqde/components/Tag.hpp>
#include <cqde/components/EntityMetaInfo.hpp>

#include <cqde/common.hpp>
#include <cqde/ecs_helpers.hpp>
#include <cqde/file_helpers.hpp>
#include <cqde/json_helpers.hpp>

#include <entt/entity/registry.hpp>

#include <entt/meta/factory.hpp>
#include <entt/meta/resolve.hpp>

#include <json/value.h>
#include <json/writer.h>

#include <fstream>


namespace cqde::types
{

const static Json::Value registryReference =
[]
{
  using ValueType = Json::ValueType;
  using namespace std::string_literals;

  Json::Value registry = ValueType::objectValue;
  registry.setComment("// entity registry must be a JSON object"s,
                       Json::CommentPlacement::commentBefore);

  Json::Value& entity = registry["cqde_json_anykey"];
  entity = ValueType::objectValue;
  entity.setComment("// entity entry must be a JSON object"s,
                    Json::CommentPlacement::commentBefore);

  Json::Value& component = entity["cqde_json_anykey"];
  component = ValueType::objectValue;
  component.setComment("// component entry must be a JSON object"s,
                        Json::CommentPlacement::commentBefore);

  return registry;
}();

void
EntityManager::load(
  const path& registryPath,
  const PackageId& packageId,
  entt::registry& registry )
{
  using fmt::format;
  using compos::Tag;
  using compos::EntityMetaInfo;
  using namespace entt::literals;

  if ( fileExists(registryPath) == false )
    return;

  Json::Value registryJson {};

  LOG_TRACE("Parsing entity registry '{}'", registryPath.string());

  try
  {
    registryJson = fileParse(registryPath);
  }
  catch ( const std::exception& e )
  {
    throw std::runtime_error(
      format("Failed to parse entity registry ({})",
              e.what()));
  }

  LOG_TRACE("Validating entity registry '{}'", registryPath.string());

  try
  {
    jsonValidateObject(registryJson, registryReference);
  }
  catch ( const std::exception& e )
  {
    throw std::runtime_error(
      format("Failed to validate entity registry '{}': {}",
              registryPath.string(), e.what()));
  }

  for ( const auto& entityId : registryJson.getMemberNames() )
  {
    if ( mEntitiesTags.count(entityId) == 0 )
      LOG_DEBUG("Importing entity '{}' ('{}')",
                entityId, registryPath.string());
    else
    {
      LOG_DEBUG("Patching entity '{}' ('{}')",
                entityId, registryPath.string());

      registry.destroy(mEntitiesTags[entityId]);
    }

    const entt::entity entity = registry.create();
    registry.emplace <Tag> (entity).id = entityId;
    mEntitiesTags[entityId] = entity;

    const auto& entityJson = registryJson[entityId];

    for ( const auto& componentName : entityJson.getMemberNames() )
    {
      LOG_TRACE("Importing component '{}' for entity '{}' ('{}')",
                componentName, entityId, registryPath.string());

      try
      {
        entt::meta_type component {};
        if ( mComponentTypes.count(componentName) != 0 )
          component = entt::resolve(mComponentTypes[componentName]);

        if ( !component )
          throw std::runtime_error(
            "Unknown component");

        const auto& componentJson = entityJson[componentName];

        auto deserializeFunc = component.func("deserialize"_hs);
        deserializeFunc.invoke( {}, entt::forward_as_meta(registry),
                                entity, entt::forward_as_meta(componentJson));
      }
      catch ( const std::exception& e )
      {
        throw std::runtime_error(
          format("Failed to import component '{}' (entity '{}'): {}",
                 componentName, entityId, e.what()));
      }
    }

    auto& metaInfo = registry.emplace_or_replace <EntityMetaInfo> (entity);
    metaInfo.packageId = packageId;
  }
}

void
EntityManager::save(
  const path& registryPath,
  const PackageId& packageId,
  const entt::registry& registry,
  const std::set <entt::id_type>& excludedComponents ) const
{
  using namespace entt::literals;

  using fmt::format;
  using compos::Tag;
  using compos::EntityMetaInfo;

  Json::Value registryJson {};

  if ( packageId.str().empty() == true )
    LOG_TRACE("Serializing global entity registry");
  else
    LOG_TRACE("Serializing package '{}' entity registry",
              packageId.str());

  for ( const auto [eTag, cTag, cMetaInfo]
          : registry.view <Tag, EntityMetaInfo> ().each() )
  {
    if (  packageId.str().empty() == false &&
          cMetaInfo.packageId != packageId )
      continue;

    const entt::entity entity = eTag;
    const std::string entityId = cTag.id.str();

    registryJson[entityId] = Json::objectValue;

    try
    {
      LOG_DEBUG("Serializing entity '{}'",
                entityId);

      each_component(entity, registry,
      [&] ( const ComponentType componentType )
      {
        if ( excludedComponents.count(componentType) != 0 )
          return true;

        const auto component = entt::resolve(componentType);
        if ( !component )
          throw std::runtime_error(
            format("Unknown component (id_type {})",
                   componentType));

        const std::string componentName = component_name(componentType);

        LOG_TRACE("Serializing component '{}' (entity '{}')",
                  componentName, entityId);

        if ( component_is_empty(componentType) == true )
        {
          registryJson[entityId][componentName] = Json::objectValue;
          return true;
        }

        const auto componentGet = component.func("get_const"_hs);
        auto componentInstance = componentGet.invoke({}, entt::forward_as_meta(registry), entity);
        const auto serializedComponent = component.func("serialize"_hs).invoke(componentInstance);

        if ( serializedComponent )
        {
          registryJson[entityId][componentName] = serializedComponent.cast <Json::Value> ();
          return true;
        }

        throw std::runtime_error(
          format("Component '{}' is not reflected properly",
                 componentName));
      });
    }
    catch ( const std::exception& e )
    {
      throw std::runtime_error(
        format("Failed to serialize entity '{}': {}",
               entityId, e.what()));
    }
  };

  if ( packageId.str().empty() == true )
    LOG_TRACE("Writing global entity registry to '{}'",
              registryPath.string());
  else
    LOG_TRACE("Writing package '{}' entity registry to '{}'",
              packageId.str(), registryPath.string());

  auto fileStream = fileOpen(registryPath, std::ios::out);
  fileStream << Json::writeString(jsonWriter(), registryJson);
}

std::string
EntityManager::componentName(
  const ComponentType type ) const
{
  for ( const auto& [name, component] : mComponentTypes )
    if ( component == type )
      return name;

  return {};
}

ComponentType
EntityManager::componentType(
  const std::string& name ) const
{
  if ( mComponentTypes.count(name) != 0 )
    return mComponentTypes.at(name);

  return {};
}

std::vector <std::string>
EntityManager::componentNames() const
{
  std::vector <std::string> names {};
  names.reserve(mComponentTypes.size());

  for ( const auto& [name, type] : mComponentTypes )
    names.push_back(name);

  return names;
}

void
EntityManager::clear()
{
  mEntitiesTags = {{null_id, entt::null}};
  mEntitesToRemove.clear();
  mComponentsToRemove.clear();
}

void
EntityManager::removeLater(
  const entt::entity entity )
{
  mEntitesToRemove.push_back(entity);
}

void
EntityManager::removeLater(
  const entt::entity  entity,
  const ComponentType component )
{
  mComponentsToRemove[entity].push_back(component);
}

void
EntityManager::delayedRemove(
  entt::registry& registry )
{
  using compos::Tag;

  for ( const auto entity : mEntitesToRemove )
  {
    mComponentsToRemove.erase(entity);

    if ( valid(entity, registry) == true )
    {
      idInvalidate( registry.get <Tag> (entity).id );
      registry.destroy(entity);
    }
  }

  for ( const auto& [entity, componentList] : mComponentsToRemove )
    if ( valid(entity, registry) == true )
      for ( const auto component : componentList )
        if ( const auto iter = registry.storage(component);
             iter < registry.storage().end() )
          iter->second.remove(entity);

  mEntitesToRemove.clear();
  mComponentsToRemove.clear();
}

bool
EntityManager::valid(
  const entt::entity    entity,
  const entt::registry& registry ) const
{
  using entity_traits = entt::entt_traits <entt::entity>;

  if ( registry.valid(entity) == false )
    return false;

  const auto versionCurrent = registry.current(entity);
  const auto versionActual = entity_traits::to_version(entity);

  return versionCurrent == versionActual;
}

entt::entity
EntityManager::get(
  const EntityId& id ) const
{
  try
  {
    return mEntitiesTags.at(id);
  }
  catch (...)
  {
    return entt::null;
  }
}

entt::entity
EntityManager::get_if_valid(
  const EntityId& id,
  const entt::registry& registry ) const
{
  const entt::entity entity = get(id);

  if ( valid(entity, registry) == true )
    return entity;

  return entt::null;
}

EntityId
EntityManager::idGenerate(
  const EntityId& hint ) const
{
  uint32_t id {};
  std::string newId {hint.str()};

  auto tokens = stringSplit(hint.str(), '_');

  if ( tokens.empty() == true )
    tokens.push_back("e");

  try
  {
    id = std::stoi(tokens.back());
  }
  catch( const std::exception& e )
  {
    tokens.push_back(std::to_string(id));
  }

  while ( mEntitiesTags.count(stringJoin(tokens, "_") ) > 0 )
    tokens.back() = std::to_string(++id);

  return stringJoin(tokens, "_");
}

void
EntityManager::idInvalidate(
  const EntityId& id )
{
  if ( id != null_id )
    mEntitiesTags.erase(id);
}

bool
EntityManager::idRegister(
  const EntityId& id,
  const entt::entity entity )
{
  if ( mEntitiesTags.count(id) == 0 )
  {
    mEntitiesTags[id] = entity;
    return true;
  }

  return false;
}

} // namespace cqde::types
