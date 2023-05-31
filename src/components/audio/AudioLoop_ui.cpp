#include <cqde/components/audio/AudioLoop.hpp>
#include <cqde/types/assets/AudioAssetManager.hpp>

#include <cqde/types/ui/widgets/StringFilter.hpp>

#include <entt/entity/registry.hpp>

#include <spdlog/fmt/bundled/format.h>

#include <imgui.h>


namespace cqde::compos
{

void
AudioLoop::ui_edit_props(
  const entt::entity entity,
  const entt::registry& registry)
{
  using fmt::format;
  using types::AudioAssetManager;

  if ( ImGui::CollapsingHeader("Layers", ImGuiTreeNodeFlags_DefaultOpen) == false )
    return;

  const auto audioList = registry.ctx().get <AudioAssetManager> ().assetIdList();

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

      audioFound = true;

      if ( ImGui::Selectable(audioId.str().c_str(), false) )
      {
        layers.push_back({audioId.str()});
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

  if ( ImGui::BeginTable("AudioList", 1, tableFlags) == false )
    return;

  ImGui::TableNextColumn();

  for ( auto iter = layers.begin();
        iter != layers.end();
        ++iter )
  {
    ImGui::PushID(std::distance(layers.begin(), iter));

    if ( ImGui::SmallButton("-##audioDel") )
      iter = layers.erase(iter);

    if ( iter == layers.end() )
    {
      ImGui::PopID();
      break;
    }

    const auto flags =  ImGuiSelectableFlags_SpanAllColumns |
                        ImGuiSelectableFlags_AllowItemOverlap;

    ImGui::SameLine();
    ImGui::Selectable(iter->source.str().c_str(), false, flags);

    ImGui::PopID();
  }

  ImGui::EndTable(); // AudioList
}

} // namespace cqde::compos
