#include <cqde/components/assets/TerrainAssetList.hpp>
#include <cqde/types/assets/TerrainAssetManager.hpp>

#include <cqde/types/ui/widgets/IdSelector.hpp>

#include <entt/entity/registry.hpp>

#include <spdlog/fmt/bundled/format.h>

#include <imgui.h>


namespace cqde::compos
{

void
TerrainAssetList::ui_edit_props(
  const entt::entity entity,
  const entt::registry& registry)
{
  using fmt::format;
  using types::TerrainAssetManager;

  if ( ImGui::CollapsingHeader("Terrain", ImGuiTreeNodeFlags_DefaultOpen) == false )
    return;

  const auto terrainAddPopupLabel {"##terrainAddPopup"};

  static ui::IdSelector terrainSelector {
    "Terrain ID", terrainAddPopupLabel };

  if ( ImGui::SmallButton("+##terrainAdd") )
    ImGui::OpenPopup(terrainAddPopupLabel);

  auto& terrainManager = registry.ctx().get <TerrainAssetManager> ();

  terrainSelector.selectPopup(
    terrainManager.assetIdList(),
    [&terrain = terrain] ( const auto& terrainId )
    {
      terrain.insert(terrainId);
    },
    [&terrain = terrain] ( const auto& terrainId )
    {
      return std::find(
        terrain.begin(),
        terrain.end(),
        terrainId ) == terrain.end();
    });


  ImGui::Separator();

  const auto tableFlags = ImGuiTableFlags_ScrollX |
                          ImGuiTableFlags_ScrollY;

  if ( ImGui::BeginTable( "TerrainToList", 1, tableFlags) == false )
    return;

  ImGui::TableNextColumn();

  for ( auto iter = terrain.begin();
        iter != terrain.end();
        ++iter )
  {
    ImGui::PushID(std::distance(terrain.begin(), iter));

    if ( ImGui::SmallButton("-##terrainDel") )
      iter = terrain.erase(iter);

    if ( iter == terrain.end() )
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

  ImGui::EndTable(); // TerrainToList
}

} // namespace cqde::compos
