#include <cqde/components/Tag.hpp>

#include <imgui.h>
#include <imgui_stdlib.h>


namespace cqde::compos
{

void
Tag::ui_edit_props(
  const entt::entity,
  const entt::registry& )
{
  if ( ImGui::CollapsingHeader("ID", ImGuiTreeNodeFlags_DefaultOpen) == false )
    return;

  std::string id = this->id.str();

  ImGui::TableNextColumn();
  if ( ImGui::InputText("##id", &id, ImGuiInputTextFlags_EnterReturnsTrue) &&
       id.empty() == false )
    this->id = id;
}

} // namespace cqde::compos
