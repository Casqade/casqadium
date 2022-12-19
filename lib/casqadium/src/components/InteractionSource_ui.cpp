#include <cqde/components/InteractionSource.hpp>

#include <imgui.h>

#include <limits>


namespace cqde::compos
{

void
InteractionSource::ui_edit_props(
  const entt::entity,
  const entt::registry& )
{
  const auto flags = ImGuiSliderFlags_AlwaysClamp |
                     ImGuiSliderFlags_NoRoundToFormat;

  if ( ImGui::CollapsingHeader("Type", ImGuiTreeNodeFlags_DefaultOpen) )
  {
    if ( ImGui::RadioButton(InteractionTypeToString(Type::ClosestVisible).c_str(),
                            type == Type::ClosestVisible) )
      type = Type::ClosestVisible;

    if ( ImGui::RadioButton(InteractionTypeToString(Type::MousePos).c_str(),
                            type == Type::MousePos) )
      type = Type::MousePos;

    if ( ImGui::RadioButton(InteractionTypeToString(Type::ViewportCenter).c_str(),
                            type == Type::ViewportCenter) )
      type = Type::ViewportCenter;
  }

  if ( ImGui::CollapsingHeader("Radius", ImGuiTreeNodeFlags_DefaultOpen) )
    ImGui::DragFloat("##radius", &radius, 0.1f,
                     -1.0f, std::numeric_limits <float>::max(),
                     "%.1f", flags);
}

} // namespace cqde::compos
