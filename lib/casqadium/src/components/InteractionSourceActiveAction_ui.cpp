#include <cqde/components/InteractionSourceActiveAction.hpp>

#include <imgui.h>
#include <imgui_stdlib.h>


namespace cqde::compos
{

void
InteractionSourceActiveAction::ui_edit_props(
  const entt::entity,
  const entt::registry& )
{
  if ( ImGui::CollapsingHeader("Action", ImGuiTreeNodeFlags_DefaultOpen) )
  {
    std::string action = actionId.str();

    ImGui::InputText("##actionId", &action);
    actionId = action;
  }
}

} // namespace cqde::compos
