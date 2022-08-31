#pragma once

#include <cqde/alias.hpp>

#include <cqde/types/ui/widgets/RegistryFilter.hpp>

#include <entt/fwd.hpp>


namespace cqde::types
{
class EntityManager;
}


namespace cqde::ui
{

class EntityManagerUi
{
  ui::RegistryFilter mRegistryFilter {};

  entt::entity  mSelectedEntity {};
  ComponentType mSelectedComponent {};

  std::string mNewEntityName {"NewEntity"};

  StringFilter mNewComponentFilter {"Component type"};

  types::EntityManager* mEntityMgr {};

public:
  EntityManagerUi( types::EntityManager* );

  void ui_show( entt::registry& );
  void ui_show_component_window( entt::registry& );

  void ui_show_scene_graph_window( entt::registry& );
};

} // namespace cqde::types
