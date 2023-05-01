#include <cqde/components/DrawableMesh.hpp>

#include <cqde/types/ui/widgets/StringFilter.hpp>
#include <cqde/types/assets/MeshAssetManager.hpp>
#include <cqde/types/assets/TextureAssetManager.hpp>

#include <entt/entity/registry.hpp>

#include <imgui.h>


namespace cqde::compos
{

void
DrawableMesh::ui_edit_props(
  const entt::entity,
  const entt::registry& registry )
{
  using ui::StringFilter;
  using types::MeshAssetManager;
  using types::TextureAssetManager;

  if ( ImGui::CollapsingHeader("Mesh ID", ImGuiTreeNodeFlags_DefaultOpen) )
  {
    static StringFilter meshFilter {"Mesh ID"};

    const auto meshes = registry.ctx().get <MeshAssetManager> ().assetIdList();

    if ( ImGui::BeginCombo("##meshId", meshId.str().c_str()) )
    {
      if ( ImGui::IsWindowAppearing() )
        ImGui::SetKeyboardFocusHere(2);

      meshFilter.search({}, ImGuiInputTextFlags_AutoSelectAll);

      for ( const auto& mesh : meshes )
      {
        if ( meshFilter.query(mesh.str()) == false )
          continue;

        const bool selected = (meshId == mesh);

        if ( ImGui::Selectable(mesh.str().c_str(), selected) )
          meshId = mesh;
      }
      ImGui::EndCombo();
    }
  }

  if ( ImGui::CollapsingHeader("Texture ID", ImGuiTreeNodeFlags_DefaultOpen) )
  {
    static StringFilter textureFilter {"Texture ID"};

    const auto textures = registry.ctx().get <TextureAssetManager> ().assetIdList();

    if ( ImGui::BeginCombo("##textureId", textureId.str().c_str()) )
    {
      if ( ImGui::IsWindowAppearing() )
        ImGui::SetKeyboardFocusHere(2);

      textureFilter.search({}, ImGuiInputTextFlags_AutoSelectAll);

      for ( const auto& texture : textures )
      {
        if ( textureFilter.query(texture.str()) == false )
          continue;

        const bool selected = (textureId == texture);

        if ( ImGui::Selectable(texture.str().c_str(), selected) )
          textureId = texture;
      }
      ImGui::EndCombo();
    }
  }
}

} // namespace cqde::compos
