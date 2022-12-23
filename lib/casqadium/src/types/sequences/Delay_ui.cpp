#include <cqde/types/sequences/Delay.hpp>

#include <entt/entity/registry.hpp>

#include <imgui.h>


namespace cqde::types
{

void
Delay::ui_show(
  const entt::registry& registry,
  const entt::entity )
{
  const auto flags = ImGuiSliderFlags_AlwaysClamp |
                     ImGuiSliderFlags_NoRoundToFormat;

  if ( ImGui::CollapsingHeader("Time", ImGuiTreeNodeFlags_DefaultOpen) == false )
    return;

  float timeElapsed {mTime.elapsed};
  float timeTotal {mTime.total};

  ImGui::Text("Time elapsed");

  ImGui::SameLine();
  ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
  if ( ImGui::DragFloat("##timeElapsed", &timeElapsed,
                        0.1f, 0.0f, timeTotal, "%.2f", flags) )
    mTime.elapsed = timeElapsed;

  ImGui::Text("Time total");

  ImGui::SameLine();
  ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
  if ( ImGui::DragFloat("##timeTotal", &timeTotal,
                        0.1f, timeElapsed,
                        std::numeric_limits <float>::max(),
                        "%.2f", flags) )
    mTime.total = timeTotal;
}

} // namespace cqde::types
