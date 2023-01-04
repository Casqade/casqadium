#include <cqde/callbacks/editor_input.hpp>
#include <cqde/callbacks/editor_common.hpp>

#include <cqde/types/EntityManager.hpp>
#include <cqde/types/SystemManager.hpp>

#include <cqde/types/ui/AssetManagerUi.hpp>
#include <cqde/types/ui/EntityManagerUi.hpp>
#include <cqde/types/ui/InputManagerUi.hpp>
#include <cqde/types/ui/PackageManagerUi.hpp>
#include <cqde/types/ui/PrefabManagerUi.hpp>
#include <cqde/types/ui/SystemManagerUi.hpp>
#include <cqde/types/ui/ViewportManagerUi.hpp>

#include <cqde/components/CasqadiumEditorInternal.hpp>

#include <soloud.h>


namespace cqde::callbacks
{

void
editorModeEnable(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  registry.ctx().get <SoLoud::Soloud> ().stopAll();

  using ui::AssetManagerUi;
  if ( registry.ctx().contains <AssetManagerUi> () == false )
    registry.ctx().emplace <AssetManagerUi> ();

  using ui::ViewportManagerUi;
  if ( registry.ctx().contains <ViewportManagerUi> () == false )
    registry.ctx().emplace <ViewportManagerUi> ();

  using ui::EntityManagerUi;
  using types::EntityManager;
  if ( registry.ctx().contains <EntityManagerUi> () == false )
  {
    auto& entityManager = registry.ctx().get <EntityManager> ();
    registry.ctx().emplace <EntityManagerUi> (&entityManager);
  }

  using ui::InputManagerUi;
  using types::InputManager;
  if ( registry.ctx().contains <InputManagerUi> () == false )
  {
    auto& inputManager = registry.ctx().get <InputManager> ();
    registry.ctx().emplace <InputManagerUi> (&inputManager);
  }

  using ui::PackageManagerUi;
  using types::PackageManager;
  if ( registry.ctx().contains <PackageManagerUi> () == false )
  {
    auto& packageManager = registry.ctx().get <PackageManager> ();
    registry.ctx().emplace <PackageManagerUi> (&packageManager);
  }

  using ui::PrefabManagerUi;
  using types::PrefabManager;
  if ( registry.ctx().contains <PrefabManagerUi> () == false )
  {
    auto& prefabManager = registry.ctx().get <PrefabManager> ();
    registry.ctx().emplace <PrefabManagerUi> (&prefabManager);
  }

  using ui::SystemManagerUi;
  using types::SystemManager;

  auto& systemManager = registry.ctx().get <SystemManager> ();

  if ( registry.ctx().contains <SystemManagerUi> () == false )
    registry.ctx().emplace <SystemManagerUi> (&systemManager);

  editorCameraCreate(registry, args);
  editorControllerCreate(registry, args);

  registry.ctx().get <SystemManagerUi> ().init(registry);

  systemManager.deactivate();

  using Phase = types::System::Phase;

  for ( const auto& systemId : systemManager.systems(Phase::Editor) )
    systemManager.activate(systemId);
};

void
editorModeDisable(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using types::EntityManager;
  using types::SystemManager;
  using compos::CasqadiumEditorInternal;

  using Phase = types::System::Phase;

  registry.ctx().get <SoLoud::Soloud> ().stopAll();

  auto& entityManager = registry.ctx().get <EntityManager> ();
  auto& systemManager = registry.ctx().get <SystemManager> ();

  for ( const auto&& [entity] : registry.view <CasqadiumEditorInternal> ().each() )
    entityManager.removeLater(entity);

  for ( const auto& systemId : systemManager.systemsActive(Phase::Editor) )
    systemManager.deactivate(systemId);
};

void
editorModeToggle(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using compos::CasqadiumEditorInternal;

  if ( registry.storage <CasqadiumEditorInternal> ().empty() == true )
    return editorModeEnable(registry, args);

  editorModeDisable(registry, args);
};

void
editorEntityMultipleSelectionEnable(
  entt::registry& registry,
  const std::vector <std::any>& args )
  {
  using ui::EntityManagerUi;

  registry.ctx().get <EntityManagerUi> ().setEntitiesMultipleSelection(true);
};

void
editorEntityMultipleSelectionDisable(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using ui::EntityManagerUi;

  registry.ctx().get <EntityManagerUi> ().setEntitiesMultipleSelection(false);
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
