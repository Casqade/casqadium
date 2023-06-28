#include <cqde/components/audio/AudioLoop.hpp>
#include <cqde/types/assets/AudioAssetManager.hpp>

#include <cqde/types/ui/widgets/IdSelector.hpp>

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

  const auto audioAddPopupLabel {"##audioAddPopup"};

  static ui::IdSelector audioSelector {
    "Audio ID", audioAddPopupLabel };

  if ( ImGui::SmallButton("+##audioAdd") )
    ImGui::OpenPopup(audioAddPopupLabel);

  auto& audioManager = registry.ctx().get <AudioAssetManager> ();

  audioSelector.selectPopup(
    audioManager.assetIdList(),
    [&layers = layers] ( const auto& audioId )
    {
      layers.push_back({audioId.str()});
    } );


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
