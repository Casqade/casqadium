#include <cqde/components/SequenceController.hpp>
#include <cqde/types/CallbackManager.hpp>

#include <cqde/types/ui/widgets/StringFilter.hpp>

#include <entt/entity/registry.hpp>

#include <spdlog/fmt/bundled/format.h>

#include <json/value.h>

#include <imgui.h>
#include <imgui_stdlib.h>


namespace cqde::compos
{

void
SequenceController::ui_edit_props(
  const entt::entity,
  const entt::registry& registry )
{
  using fmt::format;
  using types::CallbackManager;

  if ( ImGui::CollapsingHeader("Sequence steps", ImGuiTreeNodeFlags_DefaultOpen) == false )
    return;

  const auto callbackList = registry.ctx().at <CallbackManager> ().callbacks();

  static ui::StringFilter callbackFilter {"Callback ID"};

  if ( ImGui::SmallButton("+##stepAdd") )
    ImGui::OpenPopup("##stepAddPopup");

  if ( ImGui::BeginPopup("##stepAddPopup") )
  {
    if ( ImGui::IsWindowAppearing() )
      ImGui::SetKeyboardFocusHere(2);

    callbackFilter.search({}, ImGuiInputTextFlags_AutoSelectAll);

    bool callbacksFound {};

    for ( const auto& callbackId : callbackList )
    {
      if ( callbackFilter.query(callbackId.str()) == false )
        continue;

      callbacksFound = true;

      if ( ImGui::Selectable(callbackId.str().c_str(), false) )
      {
        steps.push_back(callbackId);
        ImGui::CloseCurrentPopup();
        break;
      }
    }

    if ( callbacksFound == false )
      ImGui::Text("No callbacks matching filter");

    ImGui::EndPopup(); // textureAddPopup
  }

  ImGui::Separator();

  const auto tableFlags = ImGuiTableFlags_ScrollX |
                          ImGuiTableFlags_ScrollY;

  if ( ImGui::BeginTable( "StepsList", 1, tableFlags) == false )
    return;

  ImGui::TableNextColumn();

  for ( auto iter = steps.begin();
        iter < steps.end();
        ++iter )
  {
    ImGui::PushID(std::distance(steps.begin(), iter));

    if ( ImGui::SmallButton("-##stepDel") )
      iter = steps.erase(iter);

    if ( iter == steps.end() )
    {
      ImGui::PopID();
      break;
    }

    const auto flags =  ImGuiSelectableFlags_SpanAllColumns |
                        ImGuiSelectableFlags_AllowItemOverlap;

    ImGui::SameLine();
    ImGui::Selectable(format("{}###", iter->str()).c_str(),
                      false, flags);

    static auto iter_dragged = steps.end();

    if ( ImGui::IsItemHovered() == true )
      ImGui::SetTooltip("Drag to reorder");

    if ( ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem) == true &&
         ImGui::IsMouseDown(ImGuiMouseButton_Left) == true )
    {
      auto mouseClickPos = ImGui::GetIO().MouseClickedPos[ImGuiMouseButton_Left];

      if (  mouseClickPos.x > ImGui::GetItemRectMin().x &&
            mouseClickPos.x < ImGui::GetItemRectMax().x )
      {
        if ( iter_dragged != iter &&
             iter_dragged >= steps.begin() &&
             iter_dragged < steps.end() )
          std::swap(*iter, *iter_dragged);

        iter_dragged = iter;
      }
    }

    if ( iter_dragged != steps.end() &&
         ImGui::IsMouseReleased(ImGuiMouseButton_Left) == true )
      iter_dragged = steps.end();

    ImGui::PopID();
  }

  ImGui::EndTable(); // StepsList
}

} // namespace cqde::compos
