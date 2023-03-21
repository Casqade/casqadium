#include <cqde/systems/logic.hpp>

#include <cqde/callbacks.hpp>

#include <cqde/types/ConfigManager.hpp>

#include <cqde/types/ui/AssetManagerUi.hpp>
#include <cqde/types/ui/EntityManagerUi.hpp>
#include <cqde/types/ui/InputManagerUi.hpp>
#include <cqde/types/ui/PackageManagerUi.hpp>
#include <cqde/types/ui/PrefabManagerUi.hpp>
#include <cqde/types/ui/SystemManagerUi.hpp>
#include <cqde/types/ui/ViewportManagerUi.hpp>

#include <cqde/components/SequenceController.hpp>
#include <cqde/components/SubscriberUpdate.hpp>

#include <imgui.h>


namespace cqde::systems
{

void
EditorSystem(
  entt::registry& registry )
{
  using namespace ui;

  ImGui::PushStyleColor(ImGuiCol_WindowBg, 0);
  ImGui::DockSpaceOverViewport(NULL, ImGuiDockNodeFlags_PassthruCentralNode);
  ImGui::PopStyleColor();

  callbacks::editorBindingsAssign(registry);
  callbacks::editorCameraCreate(registry);
  callbacks::editorControllerCreate(registry);

  registry.ctx().get <PackageManagerUi> ().ui_show(registry);
  registry.ctx().get <AssetManagerUi> ().ui_show(registry);
  registry.ctx().get <PrefabManagerUi> ().ui_show(registry);
  registry.ctx().get <EntityManagerUi> ().ui_show(registry);
  registry.ctx().get <InputManagerUi> ().ui_show(registry);
  registry.ctx().get <SystemManagerUi> ().ui_show(registry);
  registry.ctx().get <ViewportManagerUi> ().ui_show(registry);
}

void
SequenceSystem(
  entt::registry& registry )
{
  using compos::SubscriberUpdate;
  using compos::SequenceController;

  for ( auto&& [entity, cSequenceController]
          : registry.view <SequenceController, SubscriberUpdate> ().each() )
  {
    if ( cSequenceController.steps.empty() == true )
      continue;

    auto step = cSequenceController.steps.front().get();

    while ( step->execute(registry, entity) == true )
    {
      cSequenceController.steps.pop_front();

      if ( cSequenceController.steps.empty() == true )
        break;

      step = cSequenceController.steps.front().get();
    }
  }
}

} // namespace cqde::systems
