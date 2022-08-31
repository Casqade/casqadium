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

  if ( ImGui::CollapsingHeader("inputs", ImGuiTreeNodeFlags_DefaultOpen) == false )
    return;

  static auto selectedAxisId = null_id;
  static bool axisWindowOpened {};
  static std::string newAxisId {};

  static auto axisConfig = Json::objectValue;

  const bool newAxisIdInvalid = newAxisId.empty() == true ||
                                newAxisId == null_id.str() ||
                                inputs.count(newAxisId) > 0;

  ImGui::BeginDisabled(newAxisIdInvalid);

  if ( ImGui::Button("+##axisAdd") )
    inputs[newAxisId] = {};

  ImGui::EndDisabled();

  ImGui::SameLine();

  ImGui::InputTextWithHint("##newAxisId", "New axis ID", &newAxisId,
                           ImGuiInputTextFlags_AutoSelectAll);

  ImGui::Separator();

  std::vector <InputAxisId> axesToRemove {};

  for ( const auto& [axisId, axis] : inputs )
  {
    ImGui::PushID(axisId.str().c_str());

    auto flags = ImGuiTreeNodeFlags_Bullet |
                 ImGuiTreeNodeFlags_NoTreePushOnOpen;

    if ( axisId == selectedAxisId )
      flags |= ImGuiTreeNodeFlags_Selected;

    ImGui::TreeNodeEx(axisId.str().c_str(), flags);

    if ( ImGui::IsItemActivated() )
    {
      selectedAxisId = axisId;

      axisWindowOpened = true;
      ImGui::SetWindowFocus("###axisEditWindow");
    }

    ImGui::SameLine();

    if ( ImGui::SmallButton("-##axisRemove") )
      axesToRemove.push_back(axisId);

    ImGui::PopID(); // axisId
  }

  for ( const auto& axisId : axesToRemove )
    inputs.erase(axisId);

  if ( axisWindowOpened == false )
    return;

  if ( selectedAxisId == null_id )
    return;

  if ( inputs.count(selectedAxisId) == 0 )
    return;

  const auto windowTitle = format("Axis '{}'###axisEditWindow",
                                  selectedAxisId.str());

  if ( ImGui::Begin(windowTitle.c_str(),
                    &axisWindowOpened, ImGuiWindowFlags_MenuBar) )
    inputs.at(selectedAxisId).ui_show(registry);

  ImGui::End(); // windowTitle
}

} // namespace cqde::compos
