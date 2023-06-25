#include <cqde/types/physics/colliders/ColliderHeightField.hpp>

#include <cqde/conversion/rp3d_glm.hpp>

#include <cqde/types/assets/TerrainAssetManager.hpp>
#include <cqde/types/ui/widgets/IdSelector.hpp>

#include <entt/entity/registry.hpp>

#include <glm/vec3.hpp>

#include <reactphysics3d/collision/shapes/HeightFieldShape.h>

#include <imgui.h>


namespace cqde::types
{

void
ColliderHeightField::ui_show(
  const entt::registry& registry )
{
  using ui::IdSelector;

  if ( ImGui::CollapsingHeader("Terrain ID", ImGuiTreeNodeFlags_DefaultOpen) )
  {
    static IdSelector terrainSelector {"Terrain ID", "##terrainId"};

    const auto& terrainManager = registry.ctx().get <TerrainAssetManager> ();

    const bool selected = terrainSelector.select(
      registry, mTerrainId, terrainManager.assetIdList() );

    if ( selected == true )
    {
      const auto terrain = terrainManager.try_get(mTerrainId);

      mShape->setMinMaxHeight(terrain->heightRange.first,
                              terrain->heightRange.second);

      mShape->setNbColumns(terrain->grid.columns);
      mShape->setNbRows(terrain->grid.rows);

      mShape->setHeightFieldData(terrain->data.data());

      mShape->computeLocalAABB();
    }
  }

  if ( ImGui::CollapsingHeader("Scale", ImGuiTreeNodeFlags_DefaultOpen) )
  {
    const auto flags = ImGuiSliderFlags_NoRoundToFormat |
                       ImGuiSliderFlags_AlwaysClamp;

    glm::vec3 scale = rp3dToGlm(mShape->getScale());

    const auto scalePrev = scale;

    ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);

    ImGui::DragFloat("##hfieldScaleX", &scale.x,
                     0.01f, 0.0f, 0.0f,
                     "X: %.2f", flags);

    ImGui::DragFloat("##hfieldScaleY", &scale.y,
                     0.01f, 0.0f, 0.0f,
                     "Y: %.2f", flags);

    ImGui::DragFloat("##hfieldScaleZ", &scale.z,
                     0.01f, 0.0f, 0.0f,
                     "Z: %.2f", flags);

    ImGui::PopItemWidth();

    if ( ImGui::Button("Reset##hfieldScaleReset") )
      scale = glm::vec3{1.0f};

    if ( scale != scalePrev )
      mShape->setScale(glmToRp3d(scale));
  }

  Collider::ui_show(registry);
}

} // namespace cqde::types
