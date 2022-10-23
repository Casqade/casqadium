#include <cqde/types/physics/colliders/ColliderBox.hpp>

#include <cqde/common.hpp>
#include <cqde/conversion/json_glm_vec3.hpp>
#include <cqde/conversion/rp3d_glm.hpp>

#include <glm/mat4x4.hpp>

#include <reactphysics3d/engine/PhysicsCommon.h>
#include <reactphysics3d/collision/shapes/BoxShape.h>

#include <spdlog/fmt/bundled/format.h>

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

    glm::vec3 halfExtents {1.0f};

    if ( mShape == nullptr )
      halfExtents << mState["shape"]["halfExtents"];
    else
      halfExtents = rp3dToGlm(mShape->getHalfExtents());

    const auto halfExtentsPrev = halfExtents;

    ImGui::AlignTextToFramePadding();
    ImGui::Text("X");

    ImGui::SameLine();
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
    ImGui::DragFloat("##halfExtentsX", &halfExtents.x,
                     0.01f, 0.0f, 0.0f, "%.3f", flags);

    ImGui::AlignTextToFramePadding();
    ImGui::Text("Y");

    ImGui::SameLine();
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
    ImGui::DragFloat("##halfExtentsY", &halfExtents.y,
                     0.01f, 0.0f, 0.0f, "%.3f", flags);

    ImGui::AlignTextToFramePadding();
    ImGui::Text("Z");

    ImGui::SameLine();
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
    ImGui::DragFloat("##halfExtentsZ", &halfExtents.z,
                     0.01f, 0.0f, 0.0f, "%.3f", flags);

    if ( ImGui::Button("Reset##halfExtentsReset") )
      halfExtents = glm::vec3{1.0f};

    if ( halfExtents != halfExtentsPrev )
    {
      if ( mShape == nullptr )
      {
        auto& jsonHalfExtents = mState["shape"]["halfExtents"];

        jsonHalfExtents.clear();
        jsonHalfExtents << halfExtents;
      }
      else
        mShape->setHalfExtents(glmToRp3d(halfExtents));
    }
  }

  Collider::ui_show(registry);
}

} // namespace cqde::types
