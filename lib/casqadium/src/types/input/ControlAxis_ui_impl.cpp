#include <cqde/types/input/ControlAxis.hpp>

#include <cqde/types/CallbackManager.hpp>
#include <cqde/types/ui/widgets/StringFilter.hpp>

#include <entt/entity/registry.hpp>

#include <imgui.h>

#include <json/value.h>


namespace cqde::types
{

void
ControlAxis::ui_show(
  const entt::registry& registry )
{
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
    ImGui::SetNextItemWidth(width / 2);
    ImGui::DragFloat("Min##constraintFirst", &constraint.first, 0.1f,
                     std::numeric_limits <float>::lowest(), constraint.second, "%.3f",
                     ImGuiSliderFlags_NoRoundToFormat);

    ImGui::SetNextItemWidth(width / 2);
    ImGui::DragFloat("Max##constraintSecond", &constraint.second, 0.1f,
                     constraint.first, std::numeric_limits <float>::max(), "%.3f",
                     ImGuiSliderFlags_NoRoundToFormat);
  }

  if ( ImGui::CollapsingHeader("Callbacks", ImGuiTreeNodeFlags_DefaultOpen) )
  {
    std::vector <CallbackId> callbacksToRemove {};

    for ( const auto& callbackId : callbacks )
    {
      ImGui::PushID(callbackId.str().c_str());

      ImGui::BulletText("%s", callbackId.str().c_str());

      ImGui::SameLine();

      if ( ImGui::SmallButton("-##callbackRemove") )
        callbacksToRemove.push_back(callbackId);

      ImGui::PopID(); // callbackId
    }

    for ( const auto& callbackId : callbacksToRemove )
      callbacks.erase(callbackId);

    if ( ImGui::SmallButton("+##callbackAdd") )
      ImGui::OpenPopup("##callbackAddPopup");

    ImGui::Spacing();

    if ( ImGui::BeginPopup("##callbackAddPopup") )
    {
      static ui::StringFilter callbackFilter {"Callback ID"};

      if ( ImGui::IsWindowAppearing() )
        ImGui::SetKeyboardFocusHere(2);

      callbackFilter.search({}, ImGuiInputTextFlags_AutoSelectAll);

      auto& callbackManager = registry.ctx().at <CallbackManager> ();

      for ( const auto& callbackId : callbackManager.callbacks() )
      {
        if ( callbacks.count(callbackId) > 0 )
          continue;

        if ( callbackFilter.query(callbackId.str()) == false )
          continue;

        if ( ImGui::Selectable(callbackId.str().c_str(), false) )
        {
          callbacks.insert(callbackId);
          ImGui::CloseCurrentPopup();
          break;
        }
      }

      ImGui::EndPopup(); // callbackAddPopup
    }
  }
}

} // namespace cqde::types
