#include <cqde/components/SceneNode.hpp>

#include <imgui.h>


namespace cqde::compos
{

void
SceneNode::ui_edit_props(
  const entt::entity,
  const entt::registry& )
{
  if ( ImGui::CollapsingHeader("Parent", ImGuiTreeNodeFlags_DefaultOpen) )
    ImGui::Text("%s", parent.id.str().c_str());

  if ( ImGui::CollapsingHeader("Children", ImGuiTreeNodeFlags_DefaultOpen) )
  {
    if ( children.empty() == true )
      ImGui::Text("None");

    for ( const auto& child : children )
      ImGui::BulletText("%s", child.id.str().c_str());
  }
}

} // namespace cqde::compos
