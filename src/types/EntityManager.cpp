#include <cqde/types/EntityManager.hpp>

#include <cqde/assert.hpp>
#include <cqde/ecs_helpers.hpp>
#include <cqde/file_helpers.hpp>
#include <cqde/json_helpers.hpp>
#include <cqde/string_helpers.hpp>

#include <cqde/logger.hpp>

#include <cqde/components/Tag.hpp>
#include <cqde/components/SceneNode.hpp>
#include <cqde/components/EntityList.hpp>
#include <cqde/components/SystemList.hpp>
#include <cqde/components/EntityMetaInfo.hpp>
#include <cqde/components/SubscriberInput.hpp>
#include <cqde/components/SubscriberUpdate.hpp>
#include <cqde/components/CasqadiumEntryPoint.hpp>

#include <cqde/types/PackageManager.hpp>
#include <cqde/types/SystemManager.hpp>

#include <entt/entity/registry.hpp>

#include <entt/meta/factory.hpp>
#include <entt/meta/resolve.hpp>

#include <json/value.h>

#include <fstream>


namespace cqde::types
{

const static Json::Value registryReference =
[]
{
  using ValueType = Json::ValueType;
  using namespace std::string_literals;

  Json::Value registry = ValueType::objectValue;
  registry.setComment("// entity registry root must be a JSON object"s,
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
EntityManager::Validate(
  const Json::Value& registryJson )
{
  jsonValidateObject(registryJson, registryReference);
}

void
EntityManager::load(
  const path& registryPath,
  const PackageId& packageId,
  entt::registry& registry )
{
  using fmt::format;

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
      format("Failed to parse entity registry ({})", e.what()));
  }

  LOG_TRACE("Validating entity registry");

  try
  {
    Validate(registryJson);
  }
  catch ( const std::exception& e )
  {
    throw std::runtime_error(
      format("Failed to validate entity registry: {}", e.what()));
  }

  try
  {
    deserialize(registryJson, packageId, registry);
  }
  catch ( const std::exception& e )
  {
    throw std::runtime_error(
      format("Failed to load entity registry '{}': {}",
              registryPath.string(), e.what()));
  }
}

void
EntityManager::deserialize(
  const Json::Value& registryJson,
  const PackageId& packageId,
  entt::registry& registry )
{
  using fmt::format;
  using compos::Tag;
  using compos::EntityMetaInfo;
  using namespace entt::literals;

  LOG_DEBUG("Deserializing entity registry");

  for ( const auto& entityId : registryJson.getMemberNames() )
  {
    const auto entity = entityDeserialize(registry, entityId,
                                          registryJson[entityId]);

    if ( packageId.str().empty() == false )
    {
      auto& metaInfo = registry.emplace_or_replace <EntityMetaInfo> (entity);
      metaInfo.packageId = packageId;
    }
  }
}

Json::Value
EntityManager::serialize(
  const PackageId& packageId,
  const entt::registry& registry,
  const std::unordered_set <ComponentType>& excludedComponents ) const
{
  using namespace entt::literals;

  using fmt::format;
  using compos::Tag;
  using compos::EntityMetaInfo;

  Json::Value registryJson {Json::objectValue};

  if ( packageId.str().empty() == true )
    LOG_TRACE("Serializing global entity registry");
  else
    LOG_TRACE("Serializing package '{}' entity registry",
              packageId.str());

  for ( const auto [entity, cTag]
          : registry.view <Tag> ().each() )
  {
    if ( packageId.str().empty() == false )
    {
      const auto cMetaInfo = registry.try_get <EntityMetaInfo> (entity);

      if ( cMetaInfo == nullptr ||
           cMetaInfo->packageId != packageId )
        continue;
    }

    try
    {
      entitySerialize(registry, registryJson,
                      entity, excludedComponents);
    }
    catch ( const std::exception& e )
    {
      const auto entityId = cTag.id.str();

      throw std::runtime_error(
        format("Failed to serialize entity '{}': {}",
               entityId, e.what()));
    }
  };

  return registryJson;
}

entt::entity
EntityManager::entityCreate(
  const EntityId& id,
  entt::registry& registry )
{
  using compos::Tag;

  CQDE_ASSERT_DEBUG(id != null_id, return entt::null);

  auto entity = get_if_valid(id, registry);

  if ( entity == entt::null )
  {
    entity = registry.create();
    auto& cTag = registry.emplace <Tag> (entity);
    cTag.id = id;

    idInvalidate(id);
    idRegister(id, entity);
  }

  return entity;
}

void
EntityManager::entitySerialize(
  const entt::registry& registry,
  Json::Value& json,
  const entt::entity entity,
  const std::unordered_set <ComponentType>& excludedComponents ) const
{
  using fmt::format;
  using compos::Tag;

  const auto entityId = registry.get <Tag> (entity).id.str();

  LOG_DEBUG("Serializing entity '{}'", entityId);

  json[entityId] = Json::objectValue;

  try
  {
    each_component(entity, registry,
    [&] ( const ComponentType componentType )
    {
      if ( excludedComponents.count(componentType) != 0 )
        return true;

      componentSerialize(registry, json[entityId],
                         entity, componentType);

      return true;
    });
  }
  catch ( const std::exception& e )
  {
    throw std::runtime_error(
      format("Failed to serialize entity '{}': {}",
             entityId, e.what()));
  }
}

entt::entity
EntityManager::entityDeserialize(
  entt::registry& registry,
  EntityId entityId,
  const Json::Value& entityJson,
  const IdMap& idMap )
{
  using fmt::format;
  using compos::Tag;

  if ( entityId == null_id.str() )
    throw std::runtime_error("Failed to deserialize entity 'null': Invalid entity ID");

  if ( get(entityId) == entt::null )
    LOG_DEBUG("Deserializing entity '{}'", entityId.str());

  else if ( idMap.count(entityId) > 0 )
  {
    LOG_DEBUG("Deserializing entity '{}' as '{}'",
              entityId.str(), idMap.at(entityId).str());

    entityId = idMap.at(entityId);
  }
  else
  {
    LOG_DEBUG("Patching entity '{}'", entityId.str());

    removeLater(get(entityId));
    idInvalidate(entityId);
  }

  const auto entity = entityCreate(entityId, registry);

  for ( const auto& componentName : entityJson.getMemberNames() )
  {
    LOG_TRACE("Deserializing entity '{}' component '{}'",
              entityId.str(), componentName);

    try
    {
      componentDeserialize(registry, entity, componentName,
                           entityJson[componentName], idMap);
    }
    catch ( const std::exception& e )
    {
      throw std::runtime_error(
        format("Failed to deserialize entity '{}' component '{}': {}",
               entityId.str(), componentName, e.what()));
    }
  }

  return entity;
}

IdMap
EntityManager::prefabDeserialize(
  entt::registry& registry,
  const Json::Value& prefab,
  const PackageId& packageId )
{
  using compos::EntityMetaInfo;

  IdMap idMap {};

  for ( const auto& entityId : prefab.getMemberNames() )
  {
    idMap[entityId] = entityId;

    if ( get(entityId) != entt::null )
      idMap[entityId] = idGenerate(entityId);

    idRegister(idMap[entityId], entt::null);
  }

  for ( const auto& entityId : prefab.getMemberNames() )
  {
    const auto entity
      = entityDeserialize(registry, entityId,
                          prefab[entityId], idMap);

    auto& cMetaInfo = registry.emplace_or_replace <EntityMetaInfo> (entity);
    cMetaInfo.packageId = packageId;
  }

  return idMap;
}

void
EntityManager::componentAdd(
  const ComponentType componentType,
  const entt::entity entity,
  entt::registry& registry )
{
  using namespace entt::literals;

  const auto component = entt::resolve(componentType);
  const auto existsFunc = component.func("exists"_hs);
  const auto serializeFunc = component.func("serialize"_hs);
  auto instance = component.construct();

  if ( !instance || !existsFunc || !serializeFunc )
    return LOG_ERROR("Failed to add unresolved component '{}'",
                      componentName(componentType));

  if ( existsFunc.invoke({}, entt::forward_as_meta(registry),
                         entity).cast <bool> () == true )
    return;

  Json::Value componentJson = Json::objectValue;
  componentJson = serializeFunc.invoke(instance).cast <Json::Value> ();

  const static IdMap idMap {};

  const auto deserializeFunc = component.func("deserialize"_hs);
  deserializeFunc.invoke( {}, entt::forward_as_meta(registry),
                          entity,
                          entt::forward_as_meta(componentJson),
                          entt::forward_as_meta(idMap));
}

void
EntityManager::componentRemove(
  const ComponentType componentType,
  const entt::entity entity,
  entt::registry& registry )
{
  using namespace entt::literals;

  const auto component = entt::resolve(componentType);
  const auto removeFunc = component.func("remove"_hs);

  if ( !removeFunc )
    return LOG_ERROR("Failed to remove unresolved component '{}'",
                      componentName(componentType));

  removeFunc.invoke({}, entt::forward_as_meta(registry), entity);
}

void
EntityManager::componentSerialize(
  const entt::registry& registry,
  Json::Value& json,
  const entt::entity entity,
  const ComponentType componentType ) const
{
  using fmt::format;
  using namespace entt::literals;

  const auto component = entt::resolve(componentType);
  if ( !component )
    throw std::runtime_error(
      format("Unknown component (id_type {})",
             componentType));

  const std::string componentName = component_name(componentType);

  LOG_TRACE("Serializing component '{}'", componentName);

  if ( component_is_empty(componentType) == true )
  {
    json[componentName] = Json::objectValue;
    return;
  }

  const auto componentGet = component.func("get_const"_hs);
  auto componentInstance = componentGet.invoke({}, entt::forward_as_meta(registry), entity);
  const auto serializedComponent = component.func("serialize"_hs).invoke(componentInstance);

  if ( serializedComponent )
  {
    json[componentName] = serializedComponent.cast <Json::Value> ();
    return;
  }

  throw std::runtime_error(
    format("Component '{}' is not reflected properly",
           componentName));
}

void
EntityManager::componentDeserialize(
  entt::registry& registry,
  const entt::entity entity,
  const std::string& componentName,
  const Json::Value& componentJson,
  const IdMap& idMap )
{
  using namespace entt::literals;

  entt::meta_type component {};
  if ( mComponentTypes.count(componentName) > 0 )
    component = entt::resolve(mComponentTypes.at(componentName));

  if ( !component )
    throw std::runtime_error("Unknown component");

  const auto deserializeFunc = component.func("deserialize"_hs);
  deserializeFunc.invoke( {}, entt::forward_as_meta(registry),
                          entity,
                          entt::forward_as_meta(componentJson),
                          entt::forward_as_meta(idMap));
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
  if ( const auto iter = mComponentTypes.find(name);
       iter != mComponentTypes.end() )
    return iter->second;

  return entt::null;
}

std::vector <EntityId>
EntityManager::entities() const
{
  std::vector <EntityId> result {};
  result.reserve(mEntitiesTags.size());

  for ( const auto& [id, entity] : mEntitiesTags )
    if ( id != null_id )
      result.push_back(id);

  return result;
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
EntityManager::entryPointExecute(
  entt::registry& registry,
  const PackageId& entryPoint )
{
  using compos::CasqadiumEntryPoint;
  using compos::EntityMetaInfo;
  using compos::SubscriberInput;
  using compos::SubscriberUpdate;
  using compos::EntityList;
  using compos::SystemList;

  CQDE_ASSERT_DEBUG(entryPoint != null_id, return);
  CQDE_ASSERT_DEBUG(entryPoint.str().empty() == false, return);

  mEntryPointCurrent = entryPoint;

  const auto& packageManager = registry.ctx().get <PackageManager> ();

  for ( const auto&& [entity, cMetaInfo]
          : registry.view <CasqadiumEntryPoint, EntityMetaInfo> ().each() )
  {
    if ( cMetaInfo.packageId != entryPoint )
      continue;

    auto [cEntities, cSystems]
      = registry.try_get <const EntityList, const SystemList> (entity);

    if ( cEntities != nullptr )
      for ( const auto& entityId : cEntities->entities )
      {
        const auto entity = entityId.get(registry);

        if ( entity != entt::null )
          registry.emplace_or_replace <SubscriberUpdate> (entity);
      }

    auto& systemManager = registry.ctx().get <SystemManager> ();

    if ( cSystems != nullptr )
      for ( const auto& systemId : cSystems->systems )
        systemManager.activate(systemId);

    return;
  }
}

PackageId
EntityManager::entryPointCurrent() const
{
  return mEntryPointCurrent;
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
  using compos::SceneNode;

  for ( const auto entity : mEntitesToRemove )
  {
    mComponentsToRemove.erase(entity);

    if ( entity_valid(entity, registry) == true )
    {
      const auto& entityId = registry.get <Tag> (entity).id;

      if ( get(entityId) == entity )
      {
        if ( registry.all_of <SceneNode> (entity) == true )
          RootifyChildNode(registry, entity);

        idInvalidate(entityId);
      }

      registry.destroy(entity);
    }
  }

  for ( const auto& [entity, componentList] : mComponentsToRemove )
    if ( entity_valid(entity, registry) == true )
      for ( const auto component : componentList )
      {
        const auto componentStorage = registry.storage(component);
        if ( componentStorage != nullptr )
          componentStorage->remove(entity);
      }

  mEntitesToRemove.clear();
  mComponentsToRemove.clear();
}

entt::entity
EntityManager::get(
  const EntityId& id ) const
{
  const auto tag = mEntitiesTags.find(id);

  if ( tag != mEntitiesTags.end() )
    return tag->second;

  return entt::null;
}

entt::entity
EntityManager::get_if_valid(
  const EntityId& id,
  const entt::registry& registry ) const
{
  const entt::entity entity = get(id);

  if ( entity_valid(entity, registry) == true )
    return entity;

  return entt::null;
}

EntityId
EntityManager::idGenerate(
  const EntityId& hint ) const
{
  return IdGenerate(mEntitiesTags, hint.str(), "e");
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
