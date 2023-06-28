#include <cqde/types/sequences/CallbackExecute.hpp>

#include <cqde/types/CallbackManager.hpp>
#include <cqde/types/ui/widgets/IdSelector.hpp>

#include <entt/entity/registry.hpp>

#include <spdlog/fmt/bundled/format.h>

#include <imgui.h>


namespace cqde::types
{

void
CallbackExecute::ui_show(
  const entt::registry& registry,
  const entt::entity )
{
  using fmt::format;

  if ( ImGui::CollapsingHeader("Callbacks", ImGuiTreeNodeFlags_DefaultOpen) == false )
    return;

  const auto callbackAddPopupLabel {"##callbackAddPopup"};

  static ui::IdSelector callbackSelector {
    "Callback ID", callbackAddPopupLabel };

  if ( ImGui::SmallButton("+##callbackAdd") )
    ImGui::OpenPopup(callbackAddPopupLabel);

  auto& callbackManager = registry.ctx().get <CallbackManager> ();

  callbackSelector.selectPopup(
    callbackManager.callbacksSorted(),
    [&callbacks = mCallbacks] ( const auto& callbackId )
    {
      callbacks.push_back(callbackId);
    } );

  ImGui::Separator();

  for ( auto iter = mCallbacks.begin();
        iter < mCallbacks.end();
        ++iter )
  {
    ImGui::PushID(std::distance(mCallbacks.begin(), iter));

    if ( ImGui::SmallButton("-##callbackDel") )
      iter = mCallbacks.erase(iter);

    if ( iter == mCallbacks.end() )
    {
      ImGui::PopID();
      break;
    }

    const auto flags =  ImGuiSelectableFlags_SpanAllColumns |
                        ImGuiSelectableFlags_AllowItemOverlap;

    ImGui::SameLine();
    ImGui::Selectable(format("{}###", iter->str()).c_str(),
                      false, flags);

    static auto iter_dragged = mCallbacks.end();

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
             iter_dragged >= mCallbacks.begin() &&
             iter_dragged < mCallbacks.end() )
          std::swap(*iter, *iter_dragged);

        iter_dragged = iter;
      }
    }

    if ( iter_dragged != mCallbacks.end() &&
         ImGui::IsMouseReleased(ImGuiMouseButton_Left) == true )
      iter_dragged = mCallbacks.end();

    ImGui::PopID();
  }
}

} // namespace cqde::types
