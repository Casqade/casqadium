#include <cqde/types/physics/colliders/ColliderCapsule.hpp>

#include <reactphysics3d/collision/shapes/CapsuleShape.h>

#include <imgui.h>


namespace cqde::types
{

void
ColliderCapsule::ui_show(
  const entt::registry& registry )
{
  if ( ImGui::CollapsingHeader("Dimensions", ImGuiTreeNodeFlags_DefaultOpen) )
  {
    const auto flags = ImGuiSliderFlags_NoRoundToFormat |
                       ImGuiSliderFlags_AlwaysClamp;

    float height = mShape->getHeight();

    ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);

    if (  ImGui::DragFloat("##capsuleHeight", &height, 0.01f,
                           std::numeric_limits <float>::min(),
                           std::numeric_limits <float>::max(),
                           "Height %.3f", flags) )
      mShape->setHeight(height);

    float radius = mShape->getRadius();

    if (  ImGui::DragFloat("##capsuleRadius", &radius, 0.01f,
                           std::numeric_limits <float>::min(),
                           std::numeric_limits <float>::max(),
                           "Radius %.3f", flags) )
      mShape->setRadius(radius);

    ImGui::PopItemWidth();
  }

  Collider::ui_show(registry);
}

} // namespace cqde::types
