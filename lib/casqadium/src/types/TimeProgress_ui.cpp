#include <cqde/types/TimeProgress.hpp>

#include <imgui.h>

#include <limits>


namespace cqde::types
{

void
TimeProgress::ui_show()
{
  const auto flags = ImGuiSliderFlags_AlwaysClamp |
                     ImGuiSliderFlags_NoRoundToFormat;

  if ( ImGui::CollapsingHeader("Time", ImGuiTreeNodeFlags_DefaultOpen) == false )
    return;

  float timeElapsed {elapsed};
  float timeTotal {total};

  ImGui::Text("Time elapsed");

  ImGui::SameLine();
  ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
  if ( ImGui::DragFloat("##timeElapsed", &timeElapsed,
                        0.1f, 0.0f, timeTotal, "%.2f", flags) )
    elapsed = timeElapsed;

  ImGui::Text("Time total");

  ImGui::SameLine();
  ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
  if ( ImGui::DragFloat("##timeTotal", &timeTotal,
                        0.1f, timeElapsed,
                        std::numeric_limits <float>::max(),
                        "%.2f", flags) )
    total = timeTotal;
}

} // namespace cqde::types
