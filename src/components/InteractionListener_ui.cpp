#include <cqde/components/InteractionListener.hpp>

#include <cqde/types/CallbackManager.hpp>
#include <cqde/types/ui/widgets/StringFilter.hpp>

#include <entt/entity/registry.hpp>

#include <spdlog/fmt/bundled/format.h>

#include <imgui.h>
#include <imgui_stdlib.h>


namespace cqde::compos
{

void
InteractionListener::ui_edit_props(
  const entt::entity,
  const entt::registry& registry )
{
  using fmt::format;
  using types::CallbackManager;

  if ( ImGui::CollapsingHeader("Actions", ImGuiTreeNodeFlags_DefaultOpen) == false )
      return;

    static auto selectedActionId = null_id;
    static bool actionWindowOpened {};
    static std::string newActionId {"NewAction"};

    const bool newActionIdInvalid =
      newActionId.empty() == true ||
      newActionId == null_id.str() ||
      actions.count(newActionId) > 0;

    ImGui::BeginDisabled(newActionIdInvalid);

    if ( ImGui::Button("+##actionAdd") )
      actions[newActionId] = {};

    ImGui::EndDisabled();

    ImGui::SameLine();

    ImGui::InputTextWithHint("##newActionId", "New action ID", &newActionId,
                             ImGuiInputTextFlags_AutoSelectAll);

    ImGui::Separator();

    const auto tableFlags = ImGuiTableFlags_ScrollX |
                            ImGuiTableFlags_ScrollY;

    if ( ImGui::BeginTable( "ActionsList", 1, tableFlags) == false )
      return;

    ImGui::TableNextColumn();

    ActionId actionToRemove {};

    for ( const auto& [actionId, action] : actions )
    {
      ImGui::PushID(actionId.str().c_str());

      if ( ImGui::SmallButton("-##actionRemove") )
        actionToRemove = actionId;

      const bool selected = actionId == selectedActionId;

      const auto flags =  ImGuiSelectableFlags_SpanAllColumns |
                          ImGuiSelectableFlags_AllowItemOverlap;

      ImGui::SameLine();
      if ( ImGui::Selectable(actionId.str().c_str(),
                             selected, flags) )
      {
        selectedActionId = actionId;

        actionWindowOpened = true;
        ImGui::SetWindowFocus("###actionEditWindow");
      }

      ImGui::PopID(); // actionId
    }

    ImGui::EndTable(); // ActionsList

    if ( actionToRemove != null_id )
      actions.erase(actionToRemove);

    if ( actionWindowOpened == false )
      return;

    if ( selectedActionId == null_id )
      return;

    if ( actions.count(selectedActionId) == 0 )
      return;

    const auto windowTitle = format("Action '{}'###actionEditWindow",
                                    selectedActionId.str());

    if ( ImGui::Begin(windowTitle.c_str(),
                      &actionWindowOpened, ImGuiWindowFlags_MenuBar) == false )
      return ImGui::End(); // windowTitle

    auto& actionCallbacks = actions.at(selectedActionId);

    const auto callbacks = registry.ctx().get <CallbackManager> ().callbacks();

    static ui::StringFilter callbackFilter {"Callback ID"};

    if ( ImGui::SmallButton("+##callbackAdd") )
      ImGui::OpenPopup("##callbackAddPopup");

    if ( ImGui::BeginPopup("##callbackAddPopup") )
    {
      if ( ImGui::IsWindowAppearing() )
        ImGui::SetKeyboardFocusHere(2);

      callbackFilter.search({}, ImGuiInputTextFlags_AutoSelectAll);

      bool callbackFound {};

      for ( const auto& callbackId : callbacks )
      {
        if ( callbackFilter.query(callbackId.str()) == false )
          continue;

        if ( std::find( actionCallbacks.begin(), actionCallbacks.end(),
                        callbackId ) != actionCallbacks.end() )
          continue;

        callbackFound = true;

        if ( ImGui::Selectable(callbackId.str().c_str(), false) )
        {
          actionCallbacks.push_back(callbackId.str());
          ImGui::CloseCurrentPopup();
          break;
        }
      }

      if ( callbackFound == false )
        ImGui::Text("No callbacks matching filter");

      ImGui::EndPopup(); // callbackAddPopup
    }

    ImGui::Separator();

    if ( ImGui::BeginTable( "CallbackList", 1, tableFlags) == false )
      return ImGui::End(); // windowTitle;

    ImGui::TableNextColumn();

    for ( auto iter = actionCallbacks.begin();
          iter != actionCallbacks.end();
          ++iter )
    {
      ImGui::PushID(std::distance(actionCallbacks.begin(), iter));

      if ( ImGui::SmallButton("-##callbackDel") )
        iter = actionCallbacks.erase(iter);

      if ( iter == actionCallbacks.end() )
      {
        ImGui::PopID();
        break;
      }

      const auto flags =  ImGuiSelectableFlags_SpanAllColumns |
                          ImGuiSelectableFlags_AllowItemOverlap;

      ImGui::SameLine();
      ImGui::Selectable(iter->str().c_str(), false, flags);

      ImGui::PopID();
    }

    ImGui::EndTable(); // CallbackList

    ImGui::End(); // windowTitle
}

} // namespace cqde::compos
