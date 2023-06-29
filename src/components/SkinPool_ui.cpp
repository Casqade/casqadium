#include <cqde/components/SkinPool.hpp>

#include <cqde/types/ui/widgets/IdSelector.hpp>
#include <cqde/types/assets/MeshAssetManager.hpp>
#include <cqde/types/assets/TextureAssetManager.hpp>

#include <entt/entity/registry.hpp>

#include <imgui.h>


namespace cqde::compos
{

void
SkinPool::ui_edit_props(
  const entt::entity entity,
  const entt::registry& registry )
{
  using types::MeshAssetManager;
  using types::TextureAssetManager;

  if ( ImGui::CollapsingHeader("Skins", ImGuiTreeNodeFlags_DefaultOpen) == false )
    return;

  static MeshId selectedMeshId {};

  const auto tableFlags =
    ImGuiTableFlags_ScrollX |
    ImGuiTableFlags_ScrollY |
    ImGuiTableFlags_Resizable |
    ImGuiTableFlags_SizingStretchProp;

  if ( ImGui::BeginTable( "SkinList", 2, tableFlags ) == false )
    return;

  ImGui::TableSetupColumn("Meshes", ImGuiTableColumnFlags_NoHeaderWidth);
  ImGui::TableSetupColumn("Textures", ImGuiTableColumnFlags_NoHeaderWidth);

  ImGui::TableHeadersRow();

  ImGui::TableNextColumn();

  if ( ImGui::BeginTable( "MeshIdList", 1, tableFlags ) == false )
    return ImGui::EndTable(); // SkinList;

  ImGui::TableNextColumn();

  const auto skinAddPopupLabel {"##skinAddPopup"};

  static ui::IdSelector meshSelector {
    "Mesh ID", skinAddPopupLabel };

  if ( ImGui::SmallButton("+##skinAdd") )
    ImGui::OpenPopup(skinAddPopupLabel);

  auto& meshes = registry.ctx().get <MeshAssetManager> ();

  meshSelector.selectPopup(
    meshes.assetIdList(),
    [&skins = skins] ( const auto& meshId )
    {
      skins.insert({meshId, {}});
    },
    [&skins = skins] ( const auto& meshId )
    {
      return skins.count(meshId) == 0;
    });

  ImGui::Separator();

  std::vector <MeshId> skinsToRemove {};

  for ( const auto& [meshId, textures] : skins )
  {
    ImGui::PushID(meshId.str().c_str());

    if ( ImGui::SmallButton("-##skinRemove") )
      skinsToRemove.push_back(meshId);

    const bool selected = meshId == selectedMeshId;

    const auto flags =  ImGuiSelectableFlags_SpanAllColumns |
                        ImGuiSelectableFlags_AllowItemOverlap;

    ImGui::SameLine();
    if ( ImGui::Selectable(meshId.str().c_str(),
                           selected, flags) )
      selectedMeshId = meshId;

    ImGui::PopID(); // meshId
  }

  for ( const auto& meshId : skinsToRemove )
    skins.erase(meshId);

  ImGui::EndTable(); // MeshIdList

  if ( skins.count(selectedMeshId) == 0 )
    return ImGui::EndTable(); // SkinList

  ImGui::TableNextColumn();

  if ( ImGui::BeginTable( "TextureIdList", 1, tableFlags ) == false )
    return ImGui::EndTable(); // SkinList

  ImGui::TableNextColumn();

  const auto textureAddPopupLabel {"##textureAddPopup"};

  static ui::IdSelector textureSelector {
    "Texture ID", textureAddPopupLabel };

  if ( ImGui::SmallButton("+##textureAdd") )
    ImGui::OpenPopup(textureAddPopupLabel);

  auto& textures = registry.ctx().get <TextureAssetManager> ();

  auto& skinTextureList = skins.at(selectedMeshId);

  textureSelector.selectPopup(
    textures.assetIdList(),
    [&skinTextureList] ( const auto& textureId )
    {
      skinTextureList.push_back(textureId.str());
    },
    [&skinTextureList] ( const auto& textureId )
    {
      return std::find(
        skinTextureList.begin(),
        skinTextureList.end(),
        textureId ) == skinTextureList.end();
    });

  ImGui::Separator();

  for ( auto iter = skinTextureList.begin();
        iter != skinTextureList.end();
        ++iter )
  {
    ImGui::PushID(std::distance(skinTextureList.begin(), iter));

    if ( ImGui::SmallButton("-##textureDel") )
      iter = skinTextureList.erase(iter);

    if ( iter == skinTextureList.end() )
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

  ImGui::EndTable(); // TextureList
  ImGui::EndTable(); // SkinList
}

} // namespace cqde::compos
