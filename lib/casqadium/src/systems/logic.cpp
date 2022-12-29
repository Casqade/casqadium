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

#include <cqde/util/editor_imgui_settings.hpp>


namespace cqde::systems
{

void
EditorSystem(
  entt::registry& registry )
{
  using namespace ui;
  using types::ConfigManager;

  if ( auto& imguiIO = ImGui::GetIO();
       imguiIO.IniFilename == nullptr )
  {
    static auto imguiIniFileName = registry.ctx().at <ConfigManager> ().executableName() + ".imgui";
    imguiIO.IniFilename = imguiIniFileName.data();
    ImGui::LoadIniSettingsFromMemory(EditorImGuiSettings);
    ImGui::LoadIniSettingsFromDisk(imguiIO.IniFilename);
  }

  static int32_t frameCountPrev {};
  const auto frameCount = ImGui::GetFrameCount();

  if ( frameCountPrev == frameCount )
    return;

  frameCountPrev = frameCount;

  ImGui::PushStyleColor(ImGuiCol_WindowBg, 0);
  ImGui::DockSpaceOverViewport(NULL, ImGuiDockNodeFlags_PassthruCentralNode);
  ImGui::PopStyleColor();

  callbacks::editorBindingsAssign(registry);
  callbacks::editorCameraCreate(registry);
  callbacks::editorControllerCreate(registry);

  registry.ctx().at <PackageManagerUi> ().ui_show(registry);
  registry.ctx().at <AssetManagerUi> ().ui_show(registry);
  registry.ctx().at <PrefabManagerUi> ().ui_show(registry);
  registry.ctx().at <EntityManagerUi> ().ui_show(registry);
  registry.ctx().at <InputManagerUi> ().ui_show(registry);
  registry.ctx().at <SystemManagerUi> ().ui_show(registry);
  registry.ctx().at <ViewportManagerUi> ().ui_show(registry);
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
