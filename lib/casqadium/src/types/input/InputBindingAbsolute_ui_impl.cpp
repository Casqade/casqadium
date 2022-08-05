#include <cqde/types/input/InputBindingAbsolute.hpp>

#include <imgui.h>

#include <json/value.h>


namespace cqde::types
{

void
InputBindingAbsolute::ui_show(
  Json::Value& config )
{
  InputBinding::ui_show(config);

  if ( ImGui::CollapsingHeader("Curve", ImGuiTreeNodeFlags_DefaultOpen) == false )
    return;

  float configCurve = config["curve"].asFloat();

  ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
  if (  ImGui::DragFloat("##curve", &configCurve,
                          0.01f, 0.0f, 0.0f, "%.2f") )
    config["curve"] = configCurve;

  if ( ImGui::CollapsingHeader("Deadzone", ImGuiTreeNodeFlags_DefaultOpen) == false )
    return;

  float configDeadzone = config["deadzone"].asFloat();

  ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
  if (  ImGui::DragFloat("##deadzone", &configDeadzone,
                          0.01f, 0.0f, 0.0f, "%.2f") )
    config["deadzone"] = configDeadzone;
}

} // namespace cqde::types
