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
  using ui::IdSelector;
  using types::MeshAssetManager;
  using types::TextureAssetManager;

  if ( ImGui::CollapsingHeader("Mesh ID", ImGuiTreeNodeFlags_DefaultOpen) )
  {
    static IdSelector meshSelector {"Mesh ID", "##meshId"};

    const auto meshes = registry.ctx().get <MeshAssetManager> ().assetIdList();

    meshSelector.select(
      registry, meshId, meshes );
  }

  if ( ImGui::CollapsingHeader("Texture ID", ImGuiTreeNodeFlags_DefaultOpen) )
  {
    static IdSelector textureSelector {"Texture ID", "##textureId"};

    const auto textures = registry.ctx().get <TextureAssetManager> ().assetIdList();

    textureSelector.select(
      registry, textureId, textures );
  }
}

} // namespace cqde::compos
