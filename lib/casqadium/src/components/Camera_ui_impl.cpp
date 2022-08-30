#include <cqde/components/Camera.hpp>

#include <entt/entity/registry.hpp>

#include <glm/gtc/type_ptr.hpp>

#include <imgui.h>


namespace cqde::compos
{

void
Camera::ui_edit_props(
  const entt::entity,
  const entt::registry& )
{
  static bool fovAsRadians {};

  const float width = ImGui::GetContentRegionAvail().x / 2;

  const auto flags = ImGuiSliderFlags_AlwaysClamp | ImGuiSliderFlags_NoRoundToFormat;

  if ( ImGui::CollapsingHeader("Viewport", ImGuiTreeNodeFlags_DefaultOpen) )
  {
    ImGui::SetNextItemWidth(width);
    ImGui::DragFloat("##viewportX", &viewport.x, 0.01f,
                     0.0f, 1.0f - viewport.z + glm::epsilon <float> (),
                     "X %.2f", flags);

    ImGui::SameLine();
    ImGui::SetNextItemWidth(width);
    ImGui::DragFloat("##viewportY", &viewport.y, 0.01f,
                     0.0f, 1.0f - viewport.w + glm::epsilon <float> (),
                     "Y %.2f", flags);

    ImGui::SetNextItemWidth(width);
    ImGui::DragFloat("##viewportW", &viewport.z, 0.01f,
                     glm::epsilon <float> (), 1.0f - viewport.x,
                     "W %.2f", flags);

    ImGui::SameLine();
    ImGui::SetNextItemWidth(width);
    ImGui::DragFloat("##viewportH", &viewport.w, 0.01f,
                     glm::epsilon <float> (), 1.0f - viewport.y,
                     "H %.2f", flags);
  }

  if ( ImGui::CollapsingHeader("Z-range", ImGuiTreeNodeFlags_DefaultOpen) )
  {
    ImGui::AlignTextToFramePadding();
    ImGui::Text("zNear");

    ImGui::SameLine();
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
    ImGui::DragFloat("##zRangeNear", &zRange.first,
                      0.01f, 0.0f, zRange.second, "%.2f",
                      flags);

    ImGui::AlignTextToFramePadding();
    ImGui::Text("zFar ");

    ImGui::SameLine();
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
    ImGui::DragFloat("##zRangeFar", &zRange.second,
                      0.01f, zRange.first, std::numeric_limits <float>::max(), "%.2f",
                      flags);
  }

  if ( ImGui::CollapsingHeader("Fov", ImGuiTreeNodeFlags_DefaultOpen) )
  {
    if ( fovAsRadians == false )
    {
      float fovDegrees = glm::degrees(fov);

      if ( ImGui::DragFloat("##fov", &fovDegrees,
                            1.0f, glm::epsilon <float> (), 180.0f, "%.2f deg") )
        fov = glm::radians(fovDegrees);
    }
    else
      ImGui::DragFloat("##fov", &fov,
                       glm::radians(1.0f), glm::epsilon <float> (),
                       glm::pi <float> (), "%.3f rad");

    ImGui::SameLine();
    if ( fovAsRadians == true )
    {
      if ( ImGui::Button("rad") )
        fovAsRadians = false;
    }
    else if ( ImGui::Button("deg") )
      fovAsRadians = true;
  }

  if ( ImGui::CollapsingHeader("Projection", ImGuiTreeNodeFlags_DefaultOpen) )
  {
    if ( ImGui::RadioButton("Perspective", projectionType == Projection::Perspective) )
      projectionType = Projection::Perspective;

    if ( ImGui::RadioButton("Orthographic", projectionType == Projection::Orthographic) )
      projectionType = Projection::Orthographic;
  }

  if ( ImGui::CollapsingHeader("Render mode", ImGuiTreeNodeFlags_DefaultOpen) )
  {
    if ( ImGui::RadioButton("Wireframe", renderMode == RenderMode::Wireframe) )
      renderMode = RenderMode::Wireframe;

    if ( ImGui::RadioButton("Solid", renderMode == RenderMode::Solid) )
      renderMode = RenderMode::Solid;
  }
}

} // namespace cqde::compos
