#include <cqde/components/assets/TerrainAssetUnloadList.hpp>
#include <cqde/types/assets/TerrainAssetManager.hpp>

#include <cqde/types/ui/widgets/StringFilter.hpp>

#include <entt/entity/registry.hpp>

#include <spdlog/fmt/bundled/format.h>

#include <imgui.h>


namespace cqde::compos
{

void
TerrainAssetUnloadList::ui_edit_props(
  const entt::entity entity,
  const entt::registry& registry)
{
  using fmt::format;
  using types::TerrainAssetManager;

  if ( ImGui::CollapsingHeader("Terrain to unload", ImGuiTreeNodeFlags_DefaultOpen) == false )
    return;

  const auto terrainList = registry.ctx().at <TerrainAssetManager> ().assetIdList();

  static ui::StringFilter terrainFilter {"Terrain ID"};

  if ( ImGui::SmallButton("+##terrainAdd") )
    ImGui::OpenPopup("##terrainAddPopup");

  if ( ImGui::BeginPopup("##terrainAddPopup") )
  {
    if ( ImGui::IsWindowAppearing() )
      ImGui::SetKeyboardFocusHere(2);

    terrainFilter.search({}, ImGuiInputTextFlags_AutoSelectAll);

    bool terrainFound {};

    for ( const auto& terrainId : terrainList )
    {
      if ( terrainFilter.query(terrainId.str()) == false )
        continue;

      if ( std::find(terrainToUnload.begin(), terrainToUnload.end(),
                     terrainId) != terrainToUnload.end() )
        continue;

      terrainFound = true;

      if ( ImGui::Selectable(terrainId.str().c_str(), false) )
      {
        terrainToUnload.insert(terrainId);
        ImGui::CloseCurrentPopup();
        break;
      }
    }

    if ( terrainFound == false )
      ImGui::Text("No terrain matching filter");

    ImGui::EndPopup(); // terrainAddPopup
  }

  ImGui::Separator();

  const auto tableFlags = ImGuiTableFlags_ScrollX |
                          ImGuiTableFlags_ScrollY;

  if ( ImGui::BeginTable( "TerrainToUnloadList", 1, tableFlags) == false )
    return;

  ImGui::TableNextColumn();

  for ( auto iter = terrainToUnload.begin();
        iter != terrainToUnload.end();
        ++iter )
  {
    ImGui::PushID(std::distance(terrainToUnload.begin(), iter));

    if ( ImGui::SmallButton("-##terrainDel") )
      iter = terrainToUnload.erase(iter);

    if ( iter == terrainToUnload.end() )
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

  ImGui::EndTable(); // TerrainToUnloadList
}

} // namespace cqde::compos
