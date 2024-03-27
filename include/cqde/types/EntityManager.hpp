#pragma once

#include <cqde/alias.hpp>
#include <cqde/ecs_helpers.hpp>

#include <entt/fwd.hpp>
#include <entt/core/type_info.hpp>
#include <entt/meta/factory.hpp>

#include <json/forwards.h>

#include <filesystem>
#include <map>
#include <unordered_set>


namespace cqde::types
{

class EntityManager
{
  friend class ui::EntityManagerUi;

  using path = std::filesystem::path;

  std::unordered_map <EntityId, entt::entity,
                      identifier_hash> mEntitiesTags {{null_id, entt::null}};

  std::unordered_map <std::string, ComponentType> mComponentTypes {};

  std::vector <entt::entity> mEntitesToRemove {};
  std::unordered_map <entt::entity,
                      std::vector <ComponentType>> mComponentsToRemove {};

  PackageId mEntryPointCurrent {};

public:
  EntityManager() = default;

  static void Validate( const Json::Value& );

  void load(
    const path& registryPath,
    const PackageId&,
    entt::registry& );

  Json::Value serialize(
    const PackageId&,
    const entt::registry&,
    const std::unordered_set <ComponentType>& exclude = {} ) const;

  void deserialize(
    const Json::Value& registryJson,
    const PackageId&,
    entt::registry& );


  entt::entity entityCreate(
    const EntityId&,
    entt::registry& );

  void entitySerialize(
    const entt::registry&,
    Json::Value&,
    const entt::entity,
    const std::unordered_set <ComponentType>& exclude = {} ) const;

  entt::entity entityDeserialize(
    entt::registry&,
    EntityId,
    const Json::Value&,
    const IdMap& idMap = {} );

  IdMap prefabDeserialize(
    entt::registry&,
    const Json::Value&,
    const PackageId& );


  void componentAdd(
    const ComponentType,
    const entt::entity,
    entt::registry& );

  void componentRemove(
    const ComponentType,
    const entt::entity,
    entt::registry& );

  void componentSerialize(
    const entt::registry&,
    Json::Value&,
    const entt::entity,
    const ComponentType ) const;

  void componentDeserialize(
    entt::registry&,
    const entt::entity,
    const std::string& componentName,
    const Json::Value&,
    const IdMap& idMap = {} );


  template <typename Component>
  void registerEmptyComponent( const std::string& name );

  template <typename Component>
  void registerComponent( const std::string& name );


  template <typename Component>
  std::string componentName() const;
  std::string componentName( const ComponentType ) const;

  template <typename Component>
  ComponentType componentType() const;
  ComponentType componentType( const std::string& name ) const;

  std::vector <EntityId> entities() const;
  std::vector <std::string> componentNames() const;

  void clear();

  void entryPointExecute(
    entt::registry&,
    const PackageId& entryPoint );

  PackageId entryPointCurrent() const;

  void removeLater( const entt::entity );
  void removeLater( const entt::entity,
                    const ComponentType );

  void delayedRemove( entt::registry& );

  entt::entity get( const EntityId& ) const;
  entt::entity get_if_valid( const EntityId&,
                             const entt::registry& ) const;

  EntityId idGenerate( const EntityId& hint = {"e_"} ) const;
  void idInvalidate( const EntityId& );
  bool idRegister( const EntityId&, const entt::entity );
};

template <typename Component>
void
EntityManager::registerEmptyComponent(
  const std::string& name )
{
  using namespace entt::literals;
  using entt::type_hash;

  const auto componentType = type_hash <Component> ();

  auto factory = entt::meta <Component> ();
  factory.type(componentType);
  factory.prop("empty"_hs, true);
  factory.prop("typename"_hs, name.substr());
  factory.template func <&component_exists <Component>> ("exists"_hs);
  factory.template func <&component_remove <Component>> ("remove"_hs);
  factory.template func <&Component::serialize> ("serialize"_hs);
  factory.template func <&Component::deserialize> ("deserialize"_hs);

  mComponentTypes[name] = componentType;
}

template <typename Component>
void
EntityManager::registerComponent(
  const std::string& name )
{
  using namespace entt::literals;

  registerEmptyComponent <Component> (name);

  auto factory = entt::meta <Component> ();
  factory.prop("empty"_hs, false);
  factory.template func <&component_get <Component>, entt::as_ref_t> ("get"_hs);
  factory.template func <&component_get_const <Component>, entt::as_cref_t> ("get_const"_hs);
  factory.template func <&Component::ui_edit_props> ("ui_edit_props"_hs);
}

template <typename Component>
std::string
EntityManager::componentName() const
{
  return componentName(entt::type_hash <Component> ());
}

template <typename Component>
ComponentType
EntityManager::componentType() const
{
  return componentType( componentName <Component> () );
}

} // namespace cqde::types
