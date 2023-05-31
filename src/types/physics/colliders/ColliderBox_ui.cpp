#include <cqde/types/physics/colliders/ColliderBox.hpp>

#include <cqde/conversion/rp3d_glm.hpp>

#include <glm/vec3.hpp>

#include <reactphysics3d/collision/shapes/BoxShape.h>

#include <imgui.h>


namespace cqde::types
{

void
ColliderBox::ui_show(
  const entt::registry& registry )
{
  if ( ImGui::CollapsingHeader("Half extents", ImGuiTreeNodeFlags_DefaultOpen) )
  {
    const auto flags = ImGuiSliderFlags_NoRoundToFormat |
                       ImGuiSliderFlags_AlwaysClamp;

    glm::vec3 halfExtents = rp3dToGlm(mShape->getHalfExtents());

    const auto halfExtentsPrev = halfExtents;

    ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);

    ImGui::DragFloat("##boxHalfExtentsX",
                     &halfExtents.x, 0.01f,
                     std::numeric_limits <float>::min(),
                     std::numeric_limits <float>::max(),
                     "X: %.2f", flags);

    ImGui::DragFloat("##boxHalfExtentsY",
                     &halfExtents.y, 0.01f,
                     std::numeric_limits <float>::min(),
                     std::numeric_limits <float>::max(),
                     "Y: %.2f", flags);

    ImGui::DragFloat("##boxHalfExtentsZ",
                     &halfExtents.z, 0.01f,
                     std::numeric_limits <float>::min(),
                     std::numeric_limits <float>::max(),
                     "Z: %.2f", flags);

    ImGui::PopItemWidth();

    if ( ImGui::Button("Reset##boxHalfExtentsReset") )
      halfExtents = glm::vec3{1.0f};

    if ( halfExtents != halfExtentsPrev )
      mShape->setHalfExtents(glmToRp3d(halfExtents));
  }

  Collider::ui_show(registry);
}

} // namespace cqde::types
