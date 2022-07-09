#pragma once

#include <cqde/alias.hpp>
#include <cqde/common.hpp>
#include <cqde/ecs_helpers.hpp>

#include <entt/fwd.hpp>
#include <entt/core/type_info.hpp>
#include <entt/meta/factory.hpp>

#include <json/forwards.h>

#include <filesystem>
#include <map>
#include <set>


namespace cqde::types
{

class EntityManager
{
  std::map <EntityId, entt::entity> tags {{null_id, entt::null}};
  std::map <std::string, ComponentType> mComponentTypes {};

public:
  EntityManager() = default;

  void load(  const std::filesystem::path& registryPath,
              const PackageId&,
              entt::registry& );

  void save( const std::filesystem::path& registryPath,
             const PackageId&,
             const entt::registry&,
             const std::set <entt::id_type>& excludedComponents ) const;

  template <typename Component>
  void registerComponent( const std::string& name );

  std::string   componentName( const ComponentType ) const;
  ComponentType componentType( const std::string& name ) const;

  entt::entity get( const EntityId& ) const;
  EntityId createId( EntityId hint = {"e_"} ) const;
};

template <typename Component>
void EntityManager::registerComponent( const std::string& name )
{
  using namespace entt::literals;

  auto factory = entt::meta <Component> ();
  factory.type();
  factory.prop("typename"_hs, name.substr());
  factory.template func <&component_get <Component>, entt::as_ref_t> ("get"_hs);
  factory.template func <&component_get_const <Component>, entt::as_cref_t> ("get_const"_hs);
  factory.template func <&Component::serialize> ("serialize"_hs);
  factory.template func <&Component::deserialize> ("deserialize"_hs);

  mComponentTypes[name] = entt::type_hash <Component>::value();
}

} // namespace cqde::types
