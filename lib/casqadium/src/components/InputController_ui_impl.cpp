#include <cqde/components/InputController.hpp>

#include <entt/entity/registry.hpp>

#include <spdlog/fmt/bundled/format.h>

#include <json/value.h>

#include <imgui.h>
#include <imgui_stdlib.h>


namespace cqde::compos
{

void
InputController::ui_edit_props(
  const entt::entity,
  const entt::registry& registry )
{
  using fmt::format;

  if ( ImGui::CollapsingHeader("Axes", ImGuiTreeNodeFlags_DefaultOpen) == false )
    return;

  static auto selectedAxisId = null_id;
  static bool axisWindowOpened {};
  static std::string newAxisId {};

  static auto axisConfig = Json::objectValue;

  const bool newAxisIdInvalid = newAxisId.empty() == true ||
                                newAxisId == null_id.str() ||
                                axes.count(newAxisId) > 0;

  ImGui::BeginDisabled(newAxisIdInvalid);

  if ( ImGui::Button("+##axisAdd") )
    axes[newAxisId] = {};

  ImGui::EndDisabled();

  ImGui::SameLine();

  ImGui::InputTextWithHint("##newAxisId", "New axis ID", &newAxisId,
                           ImGuiInputTextFlags_AutoSelectAll);

  ImGui::Separator();

  const auto tableFlags = ImGuiTableFlags_ScrollX |
                          ImGuiTableFlags_ScrollY;

  if ( ImGui::BeginTable( "InputAxesList", 1, tableFlags) == false )
    return;

  ImGui::TableNextColumn();

  std::vector <InputAxisId> axesToRemove {};

  for ( const auto& [axisId, axis] : axes )
  {
    ImGui::PushID(axisId.str().c_str());

    if ( ImGui::SmallButton("-##axisRemove") )
      axesToRemove.push_back(axisId);

    const bool selected = axisId == selectedAxisId;

    const auto flags =  ImGuiSelectableFlags_SpanAllColumns |
                        ImGuiSelectableFlags_AllowItemOverlap;

    ImGui::SameLine();
    if ( ImGui::Selectable(axisId.str().c_str(),
                           selected, flags) )
    {
      selectedAxisId = axisId;

      axisWindowOpened = true;
      ImGui::SetWindowFocus("###axisEditWindow");
    }

    ImGui::PopID(); // axisId
  }

  ImGui::EndTable(); // InputAxesList

  for ( const auto& axisId : axesToRemove )
    axes.erase(axisId);

  if ( axisWindowOpened == false )
    return;

  if ( selectedAxisId == null_id )
    return;

  if ( axes.count(selectedAxisId) == 0 )
    return;

  const auto windowTitle = format("Axis '{}'###axisEditWindow",
                                  selectedAxisId.str());

  if ( ImGui::Begin(windowTitle.c_str(),
                    &axisWindowOpened, ImGuiWindowFlags_MenuBar) )
    axes.at(selectedAxisId).ui_show(registry);

  ImGui::End(); // windowTitle
}

} // namespace cqde::compos
