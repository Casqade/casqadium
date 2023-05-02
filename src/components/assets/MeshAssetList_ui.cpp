#include <cqde/components/assets/MeshAssetList.hpp>
#include <cqde/types/assets/MeshAssetManager.hpp>

#include <cqde/types/ui/widgets/StringFilter.hpp>

#include <entt/entity/registry.hpp>

#include <spdlog/fmt/bundled/format.h>

#include <imgui.h>


namespace cqde::compos
{

void
MeshAssetList::ui_edit_props(
  const entt::entity entity,
  const entt::registry& registry)
{
  using fmt::format;
  using types::MeshAssetManager;

  if ( ImGui::CollapsingHeader("Meshes", ImGuiTreeNodeFlags_DefaultOpen) == false )
    return;

  const auto meshList = registry.ctx().get <MeshAssetManager> ().assetIdList();

  static ui::StringFilter meshFilter {"Mesh ID"};

  if ( ImGui::SmallButton("+##meshAdd") )
    ImGui::OpenPopup("##meshAddPopup");

  if ( ImGui::BeginPopup("##meshAddPopup") )
  {
    if ( ImGui::IsWindowAppearing() )
      ImGui::SetKeyboardFocusHere(2);

    meshFilter.search({}, ImGuiInputTextFlags_AutoSelectAll);

    bool meshFound {};

    for ( const auto& meshId : meshList )
    {
      if ( meshFilter.query(meshId.str()) == false )
        continue;

      if ( std::find(meshes.begin(), meshes.end(),
                     meshId) != meshes.end() )
        continue;

      meshFound = true;

      if ( ImGui::Selectable(meshId.str().c_str(), false) )
      {
        meshes.insert(meshId);
        ImGui::CloseCurrentPopup();
        break;
      }
    }

    if ( meshFound == false )
      ImGui::Text("No meshes matching filter");

    ImGui::EndPopup(); // meshAddPopup
  }

  ImGui::Separator();

  const auto tableFlags = ImGuiTableFlags_ScrollX |
                          ImGuiTableFlags_ScrollY;

  if ( ImGui::BeginTable( "MeshList", 1, tableFlags) == false )
    return;

  ImGui::TableNextColumn();

  for ( auto iter = meshes.begin();
        iter != meshes.end();
        ++iter )
  {
    ImGui::PushID(std::distance(meshes.begin(), iter));

    if ( ImGui::SmallButton("-##meshDel") )
      iter = meshes.erase(iter);

    if ( iter == meshes.end() )
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

  ImGui::EndTable(); // MeshList
}

} // namespace cqde::compos
