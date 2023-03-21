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

  std::vector <entt::entity> mSelectedEntities {};
  std::pair <entt::entity, ComponentType> mSelectedComponent {};

  std::string mNewEntityName {"NewEntity"};

  StringFilter mNewComponentFilter {"Component type"};

  Json::Value mClipboard {};

  bool mEntitiesMultipleSelectionEnabled {};

  bool mEntityViewSortRequested {};

  types::EntityManager* mEntityMgr {};

public:
  EntityManagerUi( types::EntityManager* );

  void ui_show( entt::registry& );

  void setClipboard( const std::string& type,
                     const Json::Value& payload );

  void setEntitiesMultipleSelection( const bool );

  void entitySelect( const entt::entity );
  void componentSelect( const entt::entity,
                        const ComponentType );

  void entitiesDeselect();

  void entityDeselect( const entt::entity );
  void componentDeselect();

  bool entitiesMultipleSelection() const;
  bool entitySelected( const entt::entity ) const;

  std::vector <entt::entity> selectedEntities() const;
  std::pair <entt::entity, ComponentType> selectedComponent() const;

private:
  void ui_show_component_window( entt::registry& );

  void ui_show_filter_section( entt::registry& );
  void ui_show_entities_table( entt::registry& );
  void ui_show_nodes_table( entt::registry& );

  void ui_show_menu_bar( entt::registry& );

  void entitiesSave( const PackageId&, entt::registry& ) const;
};

} // namespace cqde::types
