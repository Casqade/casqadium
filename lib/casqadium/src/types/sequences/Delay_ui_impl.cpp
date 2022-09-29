#include <cqde/types/sequences/Delay.hpp>

#include <entt/entity/registry.hpp>

#include <imgui.h>


namespace cqde::types
{

void
Delay::ui_show(
  const entt::registry& registry )
{
  if ( ImGui::CollapsingHeader("Time", ImGuiTreeNodeFlags_DefaultOpen) == false )
    return;

  float timeElapsed {mTime.first};
  float timeTotal {mTime.second};

  ImGui::Text("Time elapsed");

  ImGui::SameLine();
  ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
  if ( ImGui::DragFloat("##timeElapsed", &timeElapsed,
                        0.1f, 0.0f, timeTotal) )
    mTime.first = timeElapsed;

  ImGui::Text("Time total");

  ImGui::SameLine();
  ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
  if ( ImGui::DragFloat("##timeTotal", &timeTotal,
                        0.1f, timeElapsed,
                        std::numeric_limits <float>::max() ) )
    mTime.second = timeTotal;
}

} // namespace cqde::types
