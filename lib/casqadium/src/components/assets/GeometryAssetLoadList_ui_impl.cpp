#include <cqde/components/assets/GeometryAssetLoadList.hpp>
#include <cqde/types/assets/GeometryAssetManager.hpp>

#include <cqde/types/ui/widgets/StringFilter.hpp>

#include <entt/entity/registry.hpp>

#include <spdlog/fmt/bundled/format.h>

#include <imgui.h>


namespace cqde::compos
{

void
GeometryAssetLoadList::ui_edit_props(
  const entt::entity entity,
  const entt::registry& registry)
{
  using fmt::format;
  using types::GeometryAssetManager;

  if ( ImGui::CollapsingHeader("Geometry to load", ImGuiTreeNodeFlags_DefaultOpen) == false )
    return;

  const auto geometryList = registry.ctx().at <GeometryAssetManager> ().assetIdList();

  static ui::StringFilter geometryFilter {"Geometry ID"};

  if ( ImGui::SmallButton("+##geometryAdd") )
    ImGui::OpenPopup("##geometryAddPopup");

  if ( ImGui::BeginPopup("##geometryAddPopup") )
  {
    if ( ImGui::IsWindowAppearing() )
      ImGui::SetKeyboardFocusHere(2);

    geometryFilter.search({}, ImGuiInputTextFlags_AutoSelectAll);

    bool geometryFound {};

    for ( const auto& geometryId : geometryList )
    {
      if ( geometryFilter.query(geometryId.str()) == false )
        continue;

      if ( std::find(geometryToLoad.begin(), geometryToLoad.end(),
                     geometryId) != geometryToLoad.end() )
        continue;

      geometryFound = true;

      if ( ImGui::Selectable(geometryId.str().c_str(), false) )
      {
        geometryToLoad.insert(geometryId);
        ImGui::CloseCurrentPopup();
        break;
      }
    }

    if ( geometryFound == false )
      ImGui::Text("No geometry matching filter");

    ImGui::EndPopup(); // geometryAddPopup
  }

  ImGui::Separator();

  const auto tableFlags = ImGuiTableFlags_ScrollX |
                          ImGuiTableFlags_ScrollY;

  if ( ImGui::BeginTable( "GeometryToLoadList", 1, tableFlags) == false )
    return;

  ImGui::TableNextColumn();

  for ( auto iter = geometryToLoad.begin();
        iter != geometryToLoad.end();
        ++iter )
  {
    ImGui::PushID(std::distance(geometryToLoad.begin(), iter));

    if ( ImGui::SmallButton("-##geometryDel") )
      iter = geometryToLoad.erase(iter);

    if ( iter == geometryToLoad.end() )
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

  ImGui::EndTable(); // GeometryToLoadList
}

} // namespace cqde::compos
