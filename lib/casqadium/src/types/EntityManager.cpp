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

#include <json/value.h>


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

EntityManager::EntityManager()
  : tags({{cqde::null_id, entt::null}})
{}

void
EntityManager::loadRegistry(
  const std::filesystem::path& registryPath,
  const std::string& packageTitle,
  entt::registry& registry )
{
  Json::Value registryJson {};

  LOG_TRACE("Parsing entity registry '{}'", registryPath.string());

  try
  {
    registryJson = fileParse(registryPath);
  }
  catch ( const std::exception& e )
  {
    throw std::runtime_error(format("Failed to parse entity registry ({})",
                                    e.what()));
  }

  LOG_TRACE("Validating entity registry '{}'", registryPath.string());

  try
  {
    jsonValidateObject(registryJson, registryReference);
  }
  catch ( const std::exception& e )
  {
    throw std::runtime_error(format("Failed to validate entity registry '{}': {}",
                                    registryPath.string(), e.what()));
  }

  using namespace cqde::compos;
  using namespace entt::literals;

  for ( const auto& entityId : registryJson.getMemberNames() )
  {
    entt::entity entity {};
    if ( tags.count(entityId) == 0 )
    {
      LOG_DEBUG("Importing entity '{}' ('{}')",
                entityId, registryPath.string());

      entity = registry.create();
      registry.emplace <Tag> (entity).id = entityId;
      tags[entityId] = entity;
    }
    else
      LOG_DEBUG("Patching entity '{}' ('{}')",
                entityId, registryPath.string());

    entity = tags[entityId];

    const auto& entityJson = registryJson[entityId];

    for ( const auto& componentName : entityJson.getMemberNames() )
    {
      LOG_TRACE("Importing component '{}' for entity '{}' ('{}')",
                componentName, entityId, registryPath.string());

      try
      {
        auto component = entt::resolve(identifier(componentName).hash());

        if ( !component )
          throw std::runtime_error(
            format("Unknown component",
                    componentName));

        const auto& componentJson = entityJson[componentName];

        auto deserializeFunc = component.func("deserialize"_hs);
        deserializeFunc.invoke( {}, entt::forward_as_meta(registry),
                                entity, entt::forward_as_meta(componentJson));
      }
      catch ( const std::exception& e )
      {
        throw std::runtime_error(
          format("Failed to import component '{}' for entity '{}' ('{}'): {}",
                 componentName, entityId, registryPath.string(), e.what()));
      }
    }

    auto& metaInfo = registry.emplace_or_replace <EntityMetaInfo> (entity);
    metaInfo.packageId = identifier(packageTitle).hash();
  }
}

void
EntityManager::saveRegistry(
  const std::filesystem::path& registryPath,
  const PackageId& packageId,
  entt::registry& registry )
{
  using namespace cqde::compos;
  using namespace entt::literals;

  Json::Value registryJson {};

  for ( const auto [eTag, cTag, cMetaInfo]
          : registry.view <Tag, EntityMetaInfo> ().each() )
  {
    if ( cMetaInfo.packageId != packageId.hash() )
      continue;

    const entt::entity entity = eTag;
    const std::string entityId = cTag.id.str();

    registryJson[entityId] = Json::objectValue;

    each_component(eTag, registry,
    [&] ( const ComponentType componentType )
    {
      if ( componentType == entt::type_hash <Tag>::value() ||
           componentType == entt::type_hash <EntityMetaInfo>::value() )
        return;

      auto component = entt::resolve(componentType);
      const std::string componentName = component_name(componentType);

      entt::meta_any any = component.func("get"_hs);
      any = any.invoke({}, entt::forward_as_meta(registry), entity);

      if ( any = component.func("serialize"_hs).invoke(any); any )
        registryJson[entityId][componentName] = any.cast <Json::Value> ();
    });
  };
}

entt::entity
EntityManager::get( const EntityId& id ) const
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
EntityManager::createId( EntityId hint ) const
{
  size_t idInc {};

  while ( tags.count(EntityId{(hint.str() + std::to_string(idInc)).c_str()}) > 0 )
    ++idInc;

  return EntityId{(hint.str() + std::to_string(idInc)).c_str()};
}

} // namespace cqde::types
