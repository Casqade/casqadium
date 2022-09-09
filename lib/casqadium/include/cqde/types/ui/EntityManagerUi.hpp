#pragma once

#include <cqde/alias.hpp>

#include <cqde/types/ui/widgets/RegistryFilter.hpp>

#include <entt/fwd.hpp>

#include <json/value.h>


namespace cqde::types
{
class EntityManager;
}


namespace cqde::ui
{

class EntityManagerUi
{
  RegistryFilter mRegistryFilter {};

  entt::entity  mSelectedEntity {};
  ComponentType mSelectedComponent {};

  std::string mNewEntityName {"NewEntity"};

  StringFilter mNewComponentFilter {"Component type"};

  Json::Value mClipboard {};

  types::EntityManager* mEntityMgr {};


  void ui_show_component_window( entt::registry& );

  void ui_show_filter_section( entt::registry& );
  void ui_show_entities_table( entt::registry& );
  void ui_show_nodes_table( entt::registry& );

  void prefabDeserialize( entt::registry&, const Json::Value& );

public:
  EntityManagerUi( types::EntityManager* );

  void ui_show( entt::registry& );

  void setClipboard( const std::string& type,
                     const Json::Value& payload );

  void entitySelect( const entt::entity );
  void componentSelect( const ComponentType );

  void entityDeselect();
  void componentDeselect();

  entt::entity  selectedEntity() const;
  ComponentType selectedComponent() const;
};

} // namespace cqde::types
