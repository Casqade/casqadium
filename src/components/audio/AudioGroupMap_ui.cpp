#include <cqde/components/audio/AudioGroupMap.hpp>

#include <cqde/types/ui/widgets/IdSelector.hpp>
#include <cqde/types/assets/AudioAssetManager.hpp>

#include <entt/entity/registry.hpp>

#include <spdlog/fmt/bundled/format.h>

#include <json/value.h>

#include <imgui.h>
#include <imgui_stdlib.h>


namespace cqde::compos
{

void
AudioGroupMap::ui_edit_props(
  const entt::entity,
  const entt::registry& registry )
{
  using fmt::format;
  using types::AudioAssetManager;

  if ( ImGui::CollapsingHeader("Groups", ImGuiTreeNodeFlags_DefaultOpen) == false )
    return;

  static auto selectedGroupId = null_id;
  static bool groupWindowOpened {};
  static std::string newGroupId {"NewGroup"};

  const bool newGroupIdInvalid =
    newGroupId.empty() == true ||
    newGroupId == null_id.str() ||
    groups.count(newGroupId) > 0;

  ImGui::BeginDisabled(newGroupIdInvalid);

  if ( ImGui::Button("+##groupAdd") )
    groups[newGroupId] = {};

  ImGui::EndDisabled();

  ImGui::SameLine();

  ImGui::InputTextWithHint("##newGroupId", "New group ID", &newGroupId,
                           ImGuiInputTextFlags_AutoSelectAll);

  ImGui::Separator();

  const auto tableFlags = ImGuiTableFlags_ScrollX |
                          ImGuiTableFlags_ScrollY;

  if ( ImGui::BeginTable( "AudioGroupIdList", 1, tableFlags) == false )
    return;

  ImGui::TableNextColumn();

  std::vector <AudioGroupId> groupsToRemove {};

  for ( const auto& [groupId, group] : groups )
  {
    ImGui::PushID(groupId.str().c_str());

    if ( ImGui::SmallButton("-##groupRemove") )
      groupsToRemove.push_back(groupId);

    const bool selected = groupId == selectedGroupId;

    const auto flags =  ImGuiSelectableFlags_SpanAllColumns |
                        ImGuiSelectableFlags_AllowItemOverlap;

    ImGui::SameLine();
    if ( ImGui::Selectable(groupId.str().c_str(),
                           selected, flags) )
    {
      selectedGroupId = groupId;

      groupWindowOpened = true;
      ImGui::SetWindowFocus("###audioGroupEditWindow");
    }

    ImGui::PopID(); // groupId
  }

  ImGui::EndTable(); // AudioGroupIdList

  for ( const auto& groupId : groupsToRemove )
    groups.erase(groupId);

  if ( groupWindowOpened == false )
    return;

  if ( selectedGroupId == null_id )
    return;

  if ( groups.count(selectedGroupId) == 0 )
    return;

  const auto windowTitle = format(
    "Group '{}'###audioGroupEditWindow",
    selectedGroupId.str() );

  if ( ImGui::Begin(windowTitle.c_str(),
                    &groupWindowOpened,
                    ImGuiWindowFlags_MenuBar) == false )
    return ImGui::End(); // windowTitle

  auto& audioGroupList = groups.at(selectedGroupId);

  const auto audioAddPopupLabel {"##audioAddPopup"};

  static ui::IdSelector audioSelector {
    "Audio ID", audioAddPopupLabel };

  if ( ImGui::SmallButton("+##audioAdd") )
    ImGui::OpenPopup(audioAddPopupLabel);

  auto& audioManager = registry.ctx().get <AudioAssetManager> ();

  audioSelector.selectPopup(
    audioManager.assetIdList(),
    [&audioGroupList = audioGroupList] ( const auto& audioId )
    {
      audioGroupList.push_back(audioId.str());
    },
    [&audioGroupList = audioGroupList] ( const auto& audioId )
    {
      return std::find(
        audioGroupList.begin(),
        audioGroupList.end(),
        audioId ) == audioGroupList.end();
    });


  ImGui::Separator();

  if ( ImGui::BeginTable( "AudioList", 1, tableFlags) == false )
    return ImGui::End(); // windowTitle;

  ImGui::TableNextColumn();

  for ( auto iter = audioGroupList.begin();
        iter != audioGroupList.end();
        ++iter )
  {
    ImGui::PushID(std::distance(audioGroupList.begin(), iter));

    if ( ImGui::SmallButton("-##audioDel") )
      iter = audioGroupList.erase(iter);

    if ( iter == audioGroupList.end() )
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

  ImGui::EndTable(); // AudioList

  ImGui::End(); // windowTitle
}

} // namespace cqde::compos
