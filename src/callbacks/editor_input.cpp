#include <cqde/callbacks/editor_input.hpp>

#include <cqde/types/ui/EntityManagerUi.hpp>

#include <entt/entity/registry.hpp>


namespace cqde::callbacks
{

void
editorEntityMultipleSelectionEnable(
  entt::registry& registry,
  const std::vector <std::any>& args )
  {
  using ui::EntityManagerUi;

  auto& entityManagerUi = registry.ctx().get <EntityManagerUi> ();

  entityManagerUi.setEntitiesMultipleSelection(true);
};

void
editorEntityMultipleSelectionDisable(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using ui::EntityManagerUi;

  auto& entityManagerUi = registry.ctx().get <EntityManagerUi> ();

  entityManagerUi.setEntitiesMultipleSelection(false);
};

void
editorEntityMultipleSelectionToggle(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using ui::EntityManagerUi;

  auto& entityManagerUi = registry.ctx().get <EntityManagerUi> ();

  const bool multipleSelectionEnabled = entityManagerUi.entitiesMultipleSelection();

  entityManagerUi.setEntitiesMultipleSelection( !multipleSelectionEnabled );
};

} // namespace cqde::callbacks
