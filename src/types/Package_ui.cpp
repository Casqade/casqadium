#include <cqde/types/Package.hpp>

#include <json/value.h>

#include <imgui.h>
#include <imgui_stdlib.h>


namespace cqde::types
{

void
Package::ui_show(
  Json::Value& config )
{
  std::string title = config["title"].asString();
  std::string description = config["description"].asString();
  std::string version = config["version"].asString();

  if ( ImGui::CollapsingHeader("Title", ImGuiTreeNodeFlags_DefaultOpen) )
    ImGui::InputText("##packageTitle", &title);

  if ( ImGui::CollapsingHeader("Description", ImGuiTreeNodeFlags_DefaultOpen) )
    ImGui::InputText("##packageDescription", &description);

  if ( ImGui::CollapsingHeader("Version", ImGuiTreeNodeFlags_DefaultOpen) )
    ImGui::InputText("##packageVersion", &version);

  config["title"] = title;
  config["description"] = description;
  config["version"] = version;
}

} // namespace cqde::types
