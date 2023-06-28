#include <cqde/components/assets/MeshAssetList.hpp>
#include <cqde/types/assets/MeshAssetManager.hpp>

#include <cqde/types/ui/widgets/IdSelector.hpp>

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

  const auto meshAddPopupLabel {"##meshAddPopup"};

  static ui::IdSelector meshSelector {
    "Mesh ID", meshAddPopupLabel };

  if ( ImGui::SmallButton("+##meshAdd") )
    ImGui::OpenPopup(meshAddPopupLabel);

  const auto& meshManager = registry.ctx().get <MeshAssetManager> ();

  meshSelector.selectPopup(
    meshManager.assetIdList(),
    [&meshes = meshes] ( const auto& meshId )
    {
      meshes.insert(meshId);
    },
    [&meshes = meshes] ( const auto& meshId )
    {
      return std::find(
        meshes.begin(),
        meshes.end(),
        meshId ) == meshes.end();
    });


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
