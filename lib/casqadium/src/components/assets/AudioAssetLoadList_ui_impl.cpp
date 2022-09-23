#include <cqde/components/assets/AudioAssetLoadList.hpp>
#include <cqde/types/assets/AudioAssetManager.hpp>

#include <cqde/types/ui/widgets/StringFilter.hpp>

#include <entt/entity/registry.hpp>

#include <spdlog/fmt/bundled/format.h>

#include <imgui.h>


namespace cqde::compos
{

void
AudioAssetLoadList::ui_edit_props(
  const entt::entity entity,
  const entt::registry& registry)
{
  using fmt::format;
  using types::AudioAssetManager;

  if ( ImGui::CollapsingHeader("Audio to load", ImGuiTreeNodeFlags_DefaultOpen) == false )
    return;

  const auto audioList = registry.ctx().at <AudioAssetManager> ().assetIdList();

  static ui::StringFilter audioFilter {"Audio ID"};

  if ( ImGui::SmallButton("+##audioAdd") )
    ImGui::OpenPopup("##audioAddPopup");

  if ( ImGui::BeginPopup("##audioAddPopup") )
  {
    if ( ImGui::IsWindowAppearing() )
      ImGui::SetKeyboardFocusHere(2);

    audioFilter.search({}, ImGuiInputTextFlags_AutoSelectAll);

    bool audioFound {};

    for ( const auto& audioId : audioList )
    {
      if ( audioFilter.query(audioId.str()) == false )
        continue;

      if ( std::find(audioToLoad.begin(), audioToLoad.end(),
                     audioId) != audioToLoad.end() )
        continue;

      audioFound = true;

      if ( ImGui::Selectable(audioId.str().c_str(), false) )
      {
        audioToLoad.insert(audioId);
        ImGui::CloseCurrentPopup();
        break;
      }
    }

    if ( audioFound == false )
      ImGui::Text("No audio matching filter");

    ImGui::EndPopup(); // audioAddPopup
  }

  ImGui::Separator();

  const auto tableFlags = ImGuiTableFlags_ScrollX |
                          ImGuiTableFlags_ScrollY;

  if ( ImGui::BeginTable( "AudioToLoadList", 1, tableFlags) == false )
    return;

  ImGui::TableNextColumn();

  for ( auto iter = audioToLoad.begin();
        iter != audioToLoad.end();
        ++iter )
  {
    ImGui::PushID(std::distance(audioToLoad.begin(), iter));

    if ( ImGui::SmallButton("-##audioDel") )
      iter = audioToLoad.erase(iter);

    if ( iter == audioToLoad.end() )
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

  ImGui::EndTable(); // AudioToLoadList
}

} // namespace cqde::compos
