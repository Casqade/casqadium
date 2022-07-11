#include <cqde/types/EntityManager.hpp>

#include <cqde/util/logger.hpp>

#include <cqde/components/Tag.hpp>
#include <cqde/components/EntityMetaInfo.hpp>

#include <cqde/ecs_helpers.hpp>
#include <cqde/file_helpers.hpp>
#include <cqde/json_helpers.hpp>

#include <entt/entity/registry.hpp>

#include <entt/meta/factory.hpp>
#include <entt/meta/resolve.hpp>

#include <spdlog/fmt/bundled/format.h>

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

  using namespace cqde::compos;
  using namespace entt::literals;

  for ( const auto& entityId : registryJson.getMemberNames() )
  {
    if ( tags.count(entityId) == 0 )
      LOG_DEBUG("Importing entity '{}' ('{}')",
                entityId, registryPath.string());
    else
    {
      LOG_DEBUG("Patching entity '{}' ('{}')",
                entityId, registryPath.string());

      registry.destroy(tags[entityId]);
    }

    const entt::entity entity = registry.create();
    registry.emplace <Tag> (entity).id = entityId;
    tags[entityId] = entity;

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

    if ( packageId.str().empty() == true )
      return;

    auto& metaInfo = registry.emplace_or_replace <EntityMetaInfo> (entity);
    metaInfo.packageId = packageId.hash();
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
  using cqde::compos::Tag;
  using cqde::compos::EntityMetaInfo;

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
          cMetaInfo.packageId != packageId.hash() )
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
          return;

        auto component = entt::resolve(componentType);
        if ( !component )
          throw std::runtime_error(
            format("Unknown component (id_type {})",
                   componentType));

        const std::string componentName = component_name(componentType);

        LOG_TRACE("Serializing component '{}' (entity '{}')",
                  componentName, entityId);

        auto componentGet = component.func("get_const"_hs);
        auto componentInstance = componentGet.invoke({}, entt::forward_as_meta(registry), entity);
        auto serializedComponent = component.func("serialize"_hs).invoke(componentInstance);

        if ( serializedComponent )
        {
          registryJson[entityId][componentName] = serializedComponent.cast <Json::Value> ();
          return;
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

entt::entity
EntityManager::get(
  const EntityId& id ) const
{
  try
  {
    return tags.at(id);
  }
  catch (...)
  {
    return entt::null;
  }
}

EntityId
EntityManager::createId(
  EntityId hint ) const
{
  size_t idInc {};

  while ( tags.count(EntityId{(hint.str() + std::to_string(idInc)).c_str()}) > 0 )
    ++idInc;

  return EntityId{(hint.str() + std::to_string(idInc)).c_str()};
}

} // namespace cqde::types
