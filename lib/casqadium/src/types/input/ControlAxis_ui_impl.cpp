#include <cqde/types/input/ControlAxis.hpp>

#include <cqde/types/CallbackManager.hpp>
#include <cqde/types/ui/widgets/StringFilter.hpp>

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
    ImGui::SetNextItemWidth(width);
    if ( constraint.first < constraint.second )
      ImGui::DragFloat("##value", &value, 0.1f, constraint.first, constraint.second, "%.3f",
                       ImGuiSliderFlags_NoRoundToFormat | ImGuiSliderFlags_AlwaysClamp);

    else if ( constraint.first > constraint.second )
      ImGui::DragFloat("##value", &value, 0.1f, 0.0f, 0.0f, "%.3f",
                       ImGuiSliderFlags_NoRoundToFormat | ImGuiSliderFlags_AlwaysClamp);
    else
    {
      ImGui::BeginDisabled(true);
      ImGui::DragFloat("##value", &value, 0.1f,
                       0.0f, 0.0f,
                       "%.3f", ImGuiSliderFlags_NoRoundToFormat );
      ImGui::EndDisabled();
    }
  }

  if ( ImGui::CollapsingHeader("Constraint", ImGuiTreeNodeFlags_DefaultOpen) )
  {
    ImGui::Text("Min");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
    ImGui::DragFloat("##constraintFirst", &constraint.first, 0.1f,
                     std::numeric_limits <float>::lowest(), constraint.second, "%.3f",
                     ImGuiSliderFlags_NoRoundToFormat);

    ImGui::Text("Max");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
    ImGui::DragFloat("##constraintSecond", &constraint.second, 0.1f,
                     constraint.first, std::numeric_limits <float>::max(), "%.3f",
                     ImGuiSliderFlags_NoRoundToFormat);
  }

  if ( ImGui::CollapsingHeader("Callbacks", ImGuiTreeNodeFlags_DefaultOpen) )
  {
    if ( ImGui::SmallButton("+##callbackAdd") )
      ImGui::OpenPopup("##callbackAddPopup");

    if ( ImGui::BeginPopup("##callbackAddPopup") )
    {
      static ui::StringFilter callbackFilter {"Callback ID"};

      if ( ImGui::IsWindowAppearing() )
        ImGui::SetKeyboardFocusHere(2);

      callbackFilter.search({}, ImGuiInputTextFlags_AutoSelectAll);

      auto& callbackManager = registry.ctx().at <CallbackManager> ();

      for ( const auto& callbackId : callbackManager.callbacks() )
      {
        if ( callbackFilter.query(callbackId.str()) == false )
          continue;

        if ( ImGui::Selectable(callbackId.str().c_str(), false) )
        {
          callbacks.push_back(callbackId);
          ImGui::CloseCurrentPopup();
          break;
        }
      }

      ImGui::EndPopup(); // callbackAddPopup
    }

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
