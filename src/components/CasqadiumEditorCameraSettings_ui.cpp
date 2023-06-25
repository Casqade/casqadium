#include <cqde/components/CasqadiumEditorCameraSettings.hpp>

#include <entt/entity/registry.hpp>

#include <imgui.h>


namespace cqde::compos
{

void
CasqadiumEditorCameraSettings::ui_edit_props(
  const entt::entity,
  const entt::registry& registry )
{
  if ( ImGui::CollapsingHeader("Speed", ImGuiTreeNodeFlags_DefaultOpen) == false )
    return;

  ImGui::DragFloat("##editorCameraSpeed", &speed, 0.1f);
}

} // namespace cqde::compos
