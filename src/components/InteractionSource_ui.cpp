#include <cqde/components/InteractionSource.hpp>

#include <spdlog/fmt/bundled/format.h>

#include <imgui.h>
#include <imgui_stdlib.h>

#include <limits>


namespace cqde::compos
{

void
InteractionSource::ui_edit_props(
  const entt::entity,
  const entt::registry& )
{
  using fmt::format;

  const auto flags = ImGuiSliderFlags_AlwaysClamp |
                     ImGuiSliderFlags_NoRoundToFormat;

  if ( ImGui::CollapsingHeader("Type", ImGuiTreeNodeFlags_DefaultOpen) )
  {
    if ( ImGui::RadioButton(InteractionTypeToString(Type::PhysicsProbe).c_str(),
                            type == Type::PhysicsProbe) )
      type = Type::PhysicsProbe;

    if ( ImGui::RadioButton(InteractionTypeToString(Type::ViewportCenter).c_str(),
                            type == Type::ViewportCenter) )
      type = Type::ViewportCenter;

    if ( ImGui::RadioButton(InteractionTypeToString(Type::MousePos).c_str(),
                            type == Type::MousePos) )
      type = Type::MousePos;
  }

  if ( ImGui::CollapsingHeader("Radius", ImGuiTreeNodeFlags_DefaultOpen) )
    ImGui::DragFloat("##radius", &radius, 0.1f,
                     -1.0f, std::numeric_limits <float>::max(),
                     "%.1f", flags);

  if ( ImGui::CollapsingHeader("Actions", ImGuiTreeNodeFlags_DefaultOpen) == false )
    return;

  static auto selectedActionId = null_id;
  static std::string newActionId {"NewAction"};

  const bool newActionIdInvalid =
    newActionId.empty() == true ||
    newActionId == null_id.str() ||
    actions.count(newActionId) > 0;

  ImGui::BeginDisabled(newActionIdInvalid);

  if ( ImGui::Button("+##actionAdd") )
    actions.insert(newActionId);

  ImGui::EndDisabled();

  ImGui::SameLine();

  ImGui::InputTextWithHint("##newActionId", "New action ID", &newActionId,
                           ImGuiInputTextFlags_AutoSelectAll);

  ImGui::Separator();

  const auto tableFlags = ImGuiTableFlags_ScrollX |
                          ImGuiTableFlags_ScrollY;

  if ( ImGui::BeginTable( "ActionIdList", 1, tableFlags) == false )
    return;

  ImGui::TableNextColumn();

  ActionId actionToRemove {};

  for ( const auto& actionId : actions )
  {
    ImGui::PushID(actionId.str().c_str());

    if ( ImGui::SmallButton("-##actionRemove") )
      actionToRemove = actionId;

    const auto flags =  ImGuiSelectableFlags_SpanAllColumns |
                        ImGuiSelectableFlags_AllowItemOverlap;

    ImGui::SameLine();
    ImGui::Selectable(actionId.str().c_str(), false, flags);

    ImGui::PopID(); // actionId
  }

  ImGui::EndTable(); // ActionIdList

  if ( actionToRemove != null_id )
    actions.erase(actionToRemove);
}

} // namespace cqde::compos
