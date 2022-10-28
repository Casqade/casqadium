#include <cqde/components/physics/ForceEmitter.hpp>

#include <entt/entity/registry.hpp>

#include <imgui.h>


namespace cqde::compos
{

void
ForceEmitter::ui_edit_props(
  const entt::entity entity,
  const entt::registry& registry )
{
  const auto flags = ImGuiSliderFlags_NoRoundToFormat |
                     ImGuiSliderFlags_AlwaysClamp;

  if ( ImGui::CollapsingHeader("Force", ImGuiTreeNodeFlags_DefaultOpen) )
  {
//    todo: copy from orientation

    ImGui::Checkbox("In world space##useWorldSpace", &useWorldSpace);

    ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);

    ImGui::DragFloat("##forceX", &force.x, 0.01f,
                     0.0f, 1.0f, "X: %.2f", flags);

    ImGui::DragFloat("##forceY", &force.y, 0.01f,
                     0.0f, 1.0f, "Y: %.2f", flags);

    ImGui::DragFloat("##forceZ", &force.z, 0.01f,
                     0.0f, 1.0f, "Z: %.2f", flags);

    ImGui::DragFloat("##forceAmount", &force.w, 0.1f,
                     0.0f, 0.0f, "Amount %.2f", flags);

    ImGui::PopItemWidth();

    if ( ImGui::Button("Reset##forceReset") )
      force = glm::vec4{};
  }
}

} // namespace cqde
