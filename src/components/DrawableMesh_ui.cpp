#include <cqde/components/DrawableMesh.hpp>

#include <cqde/types/ui/widgets/IdSelector.hpp>
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
  using types::MeshAssetManager;
  using types::TextureAssetManager;

  if ( ImGui::CollapsingHeader("Mesh ID", ImGuiTreeNodeFlags_DefaultOpen) )
  {
    static ui::IdSelector meshSelector {
      "Mesh ID", "##meshId" };

    auto& meshes = registry.ctx().get <MeshAssetManager> ();

    meshSelector.selectCombo(
      meshId, meshes.assetIdList() );
  }

  if ( ImGui::CollapsingHeader("Texture ID", ImGuiTreeNodeFlags_DefaultOpen) )
  {
    static ui::IdSelector textureSelector {
      "Texture ID", "##textureId" };

    auto& textures = registry.ctx().get <TextureAssetManager> ();

    textureSelector.selectCombo(
      textureId, textures.assetIdList() );
  }
}

} // namespace cqde::compos
