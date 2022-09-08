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
  StringFilter mSceneGraphFilter {"Entity ID"};

  entt::entity  mSelectedEntity {};
  ComponentType mSelectedComponent {};

  std::string mNewEntityName {"NewEntity"};
  std::string mNewNodeName {"NewSceneNode"};

  StringFilter mNewComponentFilter {"Component type"};

  Json::Value mClipboard {};

  types::EntityManager* mEntityMgr {};

public:
  EntityManagerUi( types::EntityManager* );

  void ui_show( entt::registry& );
  void ui_show_component_window( entt::registry& );

  void ui_show_scene_graph_window( entt::registry& );

  void entityPaste( entt::registry& );

  void entitySelect( const entt::entity );
  void componentSelect( const ComponentType );

  void entityDeselect();
  void componentDeselect();

  entt::entity  selectedEntity() const;
  ComponentType selectedComponent() const;
};

} // namespace cqde::types
