#include <cqde/types/ui/SystemManagerUi.hpp>
#include <cqde/types/SystemManager.hpp>

#include <imgui.h>


namespace cqde::ui
{

SystemManagerUi::SystemManagerUi(
  types::SystemManager* systemMgr )
  : mSystemMgr{systemMgr}
{}

void
SystemManagerUi::ui_show(
  entt::registry& )
{
  if ( ImGui::Begin("Systems") == false )
  {
    ImGui::End(); // Systems
    return;
  }

  using Phase = types::System::Phase;

  if ( ImGui::CollapsingHeader("Logic", ImGuiTreeNodeFlags_DefaultOpen) )
    for ( auto& system : mSystemMgr->mSystems )
      if ( system.phase == Phase::Update )
        ImGui::Checkbox(system.id.str().c_str(), &system.active);

  if ( ImGui::CollapsingHeader("Render", ImGuiTreeNodeFlags_DefaultOpen) )
    for ( auto& system : mSystemMgr->mSystems )
      if ( system.phase == Phase::Render )
        ImGui::Checkbox(system.id.str().c_str(), &system.active);

  ImGui::End(); //  Systems
}

} // namespace cqde::types
