#include <cqde/types/physics/colliders/ColliderSphere.hpp>

#include <reactphysics3d/collision/shapes/SphereShape.h>

#include <imgui.h>


namespace cqde::types
{

void
ColliderSphere::ui_show(
  const entt::registry& registry )
{
  if ( ImGui::CollapsingHeader("Dimensions", ImGuiTreeNodeFlags_DefaultOpen) )
  {
    const auto flags = ImGuiSliderFlags_NoRoundToFormat |
                       ImGuiSliderFlags_AlwaysClamp;

    float radius = mShape->getRadius();

    ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);

    if (  ImGui::DragFloat("##sphereRadius", &radius, 0.01f,
                           std::numeric_limits <float>::min(),
                           std::numeric_limits <float>::max(),
                           "Radius %.3f", flags) )
      mShape->setRadius(radius);

    ImGui::PopItemWidth();
  }

  Collider::ui_show(registry);
}

} // namespace cqde::types
