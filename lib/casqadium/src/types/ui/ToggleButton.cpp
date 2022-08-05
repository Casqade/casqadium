#include <cqde/types/ui/ToggleButton.hpp>

#include <imgui.h>


namespace cqde::ui
{

ToggleButton::ToggleButton(
  const std::string& label )
  : mLabels{label, label}
{}

ToggleButton::ToggleButton(
  const std::string& labelOn,
  const std::string& labelOff )
  : mLabels{labelOn, labelOff}
{}

bool
ToggleButton::isDown()
{
  if ( mValue == false )
  {
    ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_Button));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImGui::GetStyleColorVec4(ImGuiCol_ButtonHovered));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive));
  }
  else
  {
    ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImGui::GetStyleColorVec4(ImGuiCol_ButtonHovered));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive));
  }

  const std::string label = mValue ? mLabels.second : mLabels.first;

  const bool result = ImGui::Button(label.c_str());
  ImGui::PopStyleColor(3);

  if ( result != false )
    mValue = !mValue;

  return mValue;
}

} // namespace cqde::ui
