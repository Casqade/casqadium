#include <cqde/components/assets/AudioAssetUnloadList.hpp>
#include <cqde/types/assets/AudioAssetManager.hpp>

#include <cqde/types/ui/widgets/StringFilter.hpp>

#include <entt/entity/registry.hpp>

#include <spdlog/fmt/bundled/format.h>

#include <imgui.h>


namespace cqde::compos
{

void
AudioAssetUnloadList::ui_edit_props(
  const entt::entity entity,
  const entt::registry& registry)
{
  using fmt::format;
  using types::AudioAssetManager;

  if ( ImGui::CollapsingHeader("Audio to unload", ImGuiTreeNodeFlags_DefaultOpen) == false )
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

      if ( std::find(audioToUnload.begin(), audioToUnload.end(),
                     audioId) != audioToUnload.end() )
        continue;

      audioFound = true;

      if ( ImGui::Selectable(audioId.str().c_str(), false) )
      {
        audioToUnload.insert(audioId);
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

  if ( ImGui::BeginTable( "AudioToUnloadList", 1, tableFlags) == false )
    return;

  ImGui::TableNextColumn();

  for ( auto iter = audioToUnload.begin();
        iter != audioToUnload.end();
        ++iter )
  {
    ImGui::PushID(std::distance(audioToUnload.begin(), iter));

    if ( ImGui::SmallButton("-##audioDel") )
      iter = audioToUnload.erase(iter);

    if ( iter == audioToUnload.end() )
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

  ImGui::EndTable(); // AudioToUnloadList
}

} // namespace cqde::compos
