#include <cqde/components/assets/AudioAssetList.hpp>
#include <cqde/types/assets/AudioAssetManager.hpp>

#include <cqde/types/ui/widgets/IdSelector.hpp>

#include <entt/entity/registry.hpp>

#include <spdlog/fmt/bundled/format.h>

#include <imgui.h>


namespace cqde::compos
{

void
AudioAssetList::ui_edit_props(
  const entt::entity entity,
  const entt::registry& registry)
{
  using fmt::format;
  using types::AudioAssetManager;

  if ( ImGui::CollapsingHeader("Audio", ImGuiTreeNodeFlags_DefaultOpen) == false )
    return;

  const auto audioAddPopupLabel {"##audioAddPopup"};

  static ui::IdSelector audioSelector {
    "Audio ID", audioAddPopupLabel };

  if ( ImGui::SmallButton("+##audioAdd") )
    ImGui::OpenPopup(audioAddPopupLabel);

  auto& audioManager = registry.ctx().get <AudioAssetManager> ();

  audioSelector.selectPopup(
    audioManager.assetIdList(),
    [&audio = audio] ( const auto& audioId )
    {
      audio.insert(audioId);
    },
    [&audio = audio] ( const auto& audioId )
    {
      return std::find(
        audio.begin(),
        audio.end(),
        audioId ) == audio.end();
    });


  ImGui::Separator();

  const auto tableFlags = ImGuiTableFlags_ScrollX |
                          ImGuiTableFlags_ScrollY;

  if ( ImGui::BeginTable( "AudioList", 1, tableFlags) == false )
    return;

  ImGui::TableNextColumn();

  for ( auto iter = audio.begin();
        iter != audio.end();
        ++iter )
  {
    ImGui::PushID(std::distance(audio.begin(), iter));

    if ( ImGui::SmallButton("-##audioDel") )
      iter = audio.erase(iter);

    if ( iter == audio.end() )
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
}

} // namespace cqde::compos
