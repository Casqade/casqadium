#include <cqde/types/input/ControlAxis.hpp>

#include <cqde/types/CallbackManager.hpp>
#include <cqde/types/ui/widgets/IdSelector.hpp>

#include <entt/entity/registry.hpp>

#include <spdlog/fmt/bundled/format.h>

#include <imgui.h>

#include <json/value.h>


namespace cqde::types
{

void
ControlAxis::ui_show(
  const entt::registry& registry )
{
  using fmt::format;

  static Json::Value axisConfig = Json::objectValue;

  const float width = ImGui::GetContentRegionAvail().x;

  if ( ImGui::BeginMenuBar() )
  {
    if ( ImGui::MenuItem("Copy") )
      axisConfig = toJson();

    ImGui::BeginDisabled(axisConfig.empty() == true);

    if ( ImGui::MenuItem("Paste") )
      *this = axisConfig;

    ImGui::EndDisabled();

    ImGui::EndMenuBar();
  }

  if ( ImGui::CollapsingHeader("Value", ImGuiTreeNodeFlags_DefaultOpen) )
  {
    const auto flags = ImGuiSliderFlags_NoRoundToFormat |
                       ImGuiSliderFlags_AlwaysClamp;

    ImGui::SetNextItemWidth(width);
    if ( constraint.first < constraint.second )
      ImGui::DragFloat("##value", &value, 0.1f,
                       constraint.first,
                       constraint.second,
                       "%.3f", flags);

    else if ( constraint.first > constraint.second )
      ImGui::DragFloat("##value", &value, 0.1f,
                       0.0f, 0.0f, "%.3f", flags);
    else
    {
      ImGui::BeginDisabled(true);
      ImGui::DragFloat("##value", &value,
                       0.1f, 0.0f, 0.0f,
                       "%.3f", flags );
      ImGui::EndDisabled();
    }
  }

  if ( ImGui::CollapsingHeader("Constraint", ImGuiTreeNodeFlags_DefaultOpen) )
  {
    ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);

    ImGui::DragFloat("##constraintFirst", &constraint.first, 0.1f,
                     std::numeric_limits <float>::lowest(), constraint.second,
                     "Min: %.3f",
                     ImGuiSliderFlags_NoRoundToFormat);

    ImGui::DragFloat("##constraintSecond", &constraint.second, 0.1f,
                     constraint.first, std::numeric_limits <float>::max(),
                     "Max: %.3f",
                     ImGuiSliderFlags_NoRoundToFormat);

    ImGui::PopItemWidth();
  }

  if ( ImGui::CollapsingHeader("Callbacks", ImGuiTreeNodeFlags_DefaultOpen) )
  {
    const auto callbackAddPopupLabel {"##callbackAddPopup"};

    static ui::IdSelector callbackSelector {
      "Callback ID", callbackAddPopupLabel };

    if ( ImGui::SmallButton("+##callbackAdd") )
      ImGui::OpenPopup(callbackAddPopupLabel);

    auto& callbackManager = registry.ctx().get <CallbackManager> ();

    callbackSelector.selectPopup(
      callbackManager.callbacksSorted(),
      [&callbacks = callbacks] ( const auto& callbackId )
      {
        callbacks.push_back(callbackId);
      } );

    ImGui::Separator();

    for ( auto iter = callbacks.begin();
          iter < callbacks.end();
          ++iter )
    {
      ImGui::PushID(std::distance(callbacks.begin(), iter));

      if ( ImGui::SmallButton("-##callbackDel") )
        iter = callbacks.erase(iter);

      if ( iter == callbacks.end() )
      {
        ImGui::PopID();
        break;
      }

      const auto flags =  ImGuiSelectableFlags_SpanAllColumns |
                          ImGuiSelectableFlags_AllowItemOverlap;

      ImGui::SameLine();
      ImGui::Selectable(format("{}###", iter->str()).c_str(),
                        false, flags);

      static auto iter_dragged = callbacks.end();

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
               iter_dragged >= callbacks.begin() &&
               iter_dragged < callbacks.end() )
            std::swap(*iter, *iter_dragged);

          iter_dragged = iter;
        }
      }

      if ( iter_dragged != callbacks.end() &&
           ImGui::IsMouseReleased(ImGuiMouseButton_Left) == true )
        iter_dragged = callbacks.end();

      ImGui::PopID();
    }
  }
}

} // namespace cqde::types
