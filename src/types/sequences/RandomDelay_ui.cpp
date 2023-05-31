#include <cqde/types/sequences/RandomDelay.hpp>

#include <entt/entity/registry.hpp>

#include <imgui.h>


namespace cqde::types
{

void
RandomDelay::ui_show(
  const entt::registry& registry,
  const entt::entity entity )
{
  Delay::ui_show(registry, entity);

  const auto flags = ImGuiSliderFlags_AlwaysClamp |
                     ImGuiSliderFlags_NoRoundToFormat;

  if ( ImGui::CollapsingHeader("Delay range", ImGuiTreeNodeFlags_DefaultOpen) == false )
    return;

  float rangeMin {mDelayRange.min};
  float rangeMax {mDelayRange.max};

  ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
  if ( ImGui::DragFloat("##rangeMin", &rangeMin,
                        0.1f, 0.0f, rangeMax, "Min: %.2f", flags) )
    mDelayRange.min = rangeMin;

  ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
  if ( ImGui::DragFloat("##rangeMax", &rangeMax,
                        0.1f, rangeMin,
                        std::numeric_limits <float>::max(),
                        "Max: %.2f", flags) )
    mDelayRange.max = rangeMax;
}

} // namespace cqde::types
