#pragma once

#include <cqde/alias.hpp>
#include <cqde/ecs_helpers.hpp>

#include <entt/fwd.hpp>
#include <entt/core/type_info.hpp>
#include <entt/meta/factory.hpp>

#include <json/forwards.h>

#include <filesystem>
#include <map>
#include <set>


namespace cqde::ui
{
class EntityManagerUi;
}

namespace cqde::types
{

class EntityManager
{
  friend class ui::EntityManagerUi;

  using path = std::filesystem::path;

  std::map <EntityId, entt::entity> mEntitiesTags {{null_id, entt::null}};
  std::map <std::string, ComponentType> mComponentTypes {};

  std::set <entt::entity> mEntitesToRemove {};
  std::map <entt::entity,
            std::set <ComponentType>> mComponentsToRemove {};

public:
  EntityManager() = default;

  void load(  const path& registryPath,
              const PackageId&,
              entt::registry& );

  void save( const path& registryPath,
             const PackageId&,
             const entt::registry&,
             const std::set <entt::id_type>& excludedComponents ) const;

  template <typename Component>
  void registerComponent( const std::string& name );

  std::string   componentName( const ComponentType ) const;
  ComponentType componentType( const std::string& name ) const;

  std::vector <std::string> componentNames() const;

  void clear();

  void removeLater( const entt::entity );
  void removeLater( const entt::entity, const ComponentType );

  void delayedRemove( entt::registry& );

  entt::entity get( const EntityId& ) const;
  EntityId idGenerate( const EntityId& hint = {"e_"} ) const;
};

template <typename Component>
void
EntityManager::registerComponent(
  const std::string& name )
{
  using namespace entt::literals;
  using entt::type_hash;

  auto factory = entt::meta <Component> ();
  factory.type();
  factory.prop("typename"_hs, name.substr());
  factory.template func <&component_get <Component>, entt::as_ref_t> ("get"_hs);
  factory.template func <&component_get_const <Component>, entt::as_cref_t> ("get_const"_hs);
  factory.template func <&Component::serialize> ("serialize"_hs);
  factory.template func <&Component::deserialize> ("deserialize"_hs);
  factory.template func <&Component::ui_edit_props> ("ui_edit_props"_hs);

  mComponentTypes[name] = type_hash <Component> ();
}

} // namespace cqde::types
