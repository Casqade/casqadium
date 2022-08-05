#pragma once

#include <cqde/alias.hpp>

#include <cqde/types/ui/RegistryFilter.hpp>

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

  types::EntityManager* mEntityMgr {};

public:
  EntityManagerUi( types::EntityManager* );

  void ui_show( entt::registry& );
};

} // namespace cqde::types
