#include <cqde/types/input/InputBinding.hpp>
#include <cqde/types/input/InputBindingAbsolute.hpp>
#include <cqde/types/input/InputBindingRelative.hpp>

#include <cqde/json_helpers.hpp>

#include <imgui.h>

#include <json/value.h>


namespace cqde::types
{

void
InputBinding::ui_show(
  Json::Value& config )
{
  float configSensitivity = config["sensitivity"].asFloat();

  if ( ImGui::CollapsingHeader("Type", ImGuiTreeNodeFlags_DefaultOpen) )
  {
    if ( ImGui::RadioButton("Absolute", config["absolute"].asBool()) )
      config = InputBindingAbsolute{inputId, configSensitivity}.toJson();

    if ( ImGui::RadioButton("Relative", config["absolute"].asBool() == false) )
      config = InputBindingRelative{inputId, configSensitivity}.toJson();
  }

  if ( ImGui::CollapsingHeader("Sensitivity", ImGuiTreeNodeFlags_DefaultOpen) == false )
    return;

  ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
  if (  ImGui::DragFloat("##sensitivity", &configSensitivity,
                          0.01f, 0.0f, 0.0f, "%.2f") )
    config["sensitivity"] = configSensitivity;
}

} // namespace cqde::types
