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

  const auto flags = ImGuiSliderFlags_AlwaysClamp |
                     ImGuiSliderFlags_NoRoundToFormat;

  if ( ImGui::CollapsingHeader("Viewport", ImGuiTreeNodeFlags_DefaultOpen) )
  {
    ImGui::PushItemWidth(width);

    ImGui::DragFloat("##viewportX", &viewport.x, 0.01f,
                     0.0f, 1.0f - viewport.z + glm::epsilon <float> (),
                     "X %.2f", flags);

    ImGui::SameLine();
    ImGui::DragFloat("##viewportY", &viewport.y, 0.01f,
                     0.0f, 1.0f - viewport.w + glm::epsilon <float> (),
                     "Y %.2f", flags);

    ImGui::DragFloat("##viewportW", &viewport.z, 0.01f,
                     glm::epsilon <float> (), 1.0f - viewport.x,
                     "W %.2f", flags);

    ImGui::SameLine();
    ImGui::DragFloat("##viewportH", &viewport.w, 0.01f,
                     glm::epsilon <float> (), 1.0f - viewport.y,
                     "H %.2f", flags);

    ImGui::PopItemWidth();
  }

  if ( ImGui::CollapsingHeader("Z-range", ImGuiTreeNodeFlags_DefaultOpen) )
  {
    ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);

    ImGui::DragFloat("##zRangeNear", &zRange.first,
                      0.01f, 0.0f, zRange.second,
                      "Z near %.2f",flags);

    ImGui::DragFloat("##zRangeFar", &zRange.second,
                      0.01f, zRange.first, std::numeric_limits <float>::max(),
                     "Z far %.2f", flags);

    ImGui::PopItemWidth();
  }

  if ( ImGui::CollapsingHeader("Aspect ratio", ImGuiTreeNodeFlags_DefaultOpen) )
  {
    ImGui::DragFloat("##aspectRatio", &aspectRatio,
                     0.01f, 0.0f, std::numeric_limits <float>::max(),
                     "%.2f", flags);

    if ( ImGui::RadioButton(AspectRatioBehaviourToString(AspectRatioBehaviour::Keep).c_str(),
                            aspectRatioBehaviour == AspectRatioBehaviour::Keep) )
      aspectRatioBehaviour = AspectRatioBehaviour::Keep;

    if ( ImGui::RadioButton(AspectRatioBehaviourToString(AspectRatioBehaviour::Stretch).c_str(),
                            aspectRatioBehaviour == AspectRatioBehaviour::Stretch) )
      aspectRatioBehaviour = AspectRatioBehaviour::Stretch;

    if ( ImGui::RadioButton(AspectRatioBehaviourToString(AspectRatioBehaviour::Native).c_str(),
                            aspectRatioBehaviour == AspectRatioBehaviour::Native) )
      aspectRatioBehaviour = AspectRatioBehaviour::Native;
  }

  if ( ImGui::CollapsingHeader("Layer", ImGuiTreeNodeFlags_DefaultOpen) )
    ImGui::DragInt("##layer", &layer, 1.0f);

  const std::string headerTitle = projectionType == Projection::Perspective
                                ? "Fov" : "Scale";

  if ( ImGui::CollapsingHeader((headerTitle + "###cameraScale").c_str(), ImGuiTreeNodeFlags_DefaultOpen) )
  {
    switch (projectionType)
    {
      case Projection::Perspective:
      {
        if ( fovAsRadians == false )
        {
          float fovDegrees = glm::degrees(fov);

          if (  ImGui::DragFloat(
                  "##fov", &fovDegrees,
                  1.0f, std::numeric_limits <float>::min(),
                  180.0f, "%.2f deg", flags ) )
            fov = glm::radians(fovDegrees);
        }
        else
          ImGui::DragFloat(
            "##fov", &fov,
            glm::radians(1.0f),
            std::numeric_limits <float>::min(),
            glm::pi <float> (), "%.3f rad", flags );

        ImGui::SameLine();
        if ( fovAsRadians == true )
        {
          if ( ImGui::Button("rad") )
            fovAsRadians = false;
        }
        else if ( ImGui::Button("deg") )
          fovAsRadians = true;

        break;
      }

      case Projection::Orthographic:
      {
        ImGui::DragFloat(
          "##zoom", &scale, 0.01f,
          std::numeric_limits <float>::min(),
          std::numeric_limits <float>::max(),
          "%.2f", flags );

        break;
      }
    }
  }

  if ( ImGui::CollapsingHeader("Projection", ImGuiTreeNodeFlags_DefaultOpen) )
  {
    if ( ImGui::RadioButton(ProjectionTypeToString(Projection::Perspective).c_str(),
                            projectionType == Projection::Perspective) )
      projectionType = Projection::Perspective;

    if ( ImGui::RadioButton(ProjectionTypeToString(Projection::Orthographic).c_str(),
                            projectionType == Projection::Orthographic) )
      projectionType = Projection::Orthographic;
  }

  if ( ImGui::CollapsingHeader("Render mode", ImGuiTreeNodeFlags_DefaultOpen) )
  {
    if ( ImGui::RadioButton(RenderModeToString(RenderMode::Wireframe).c_str(),
                            renderMode == RenderMode::Wireframe) )
      renderMode = RenderMode::Wireframe;

    if ( ImGui::RadioButton(RenderModeToString(RenderMode::Solid).c_str(),
                            renderMode == RenderMode::Solid) )
      renderMode = RenderMode::Solid;
  }

  if ( ImGui::CollapsingHeader("Texture mode", ImGuiTreeNodeFlags_DefaultOpen) )
  {
    if ( ImGui::RadioButton(TextureModeToString(TextureMode::Textured).c_str(),
                            textureMode == TextureMode::Textured) )
      textureMode = TextureMode::Textured;

    if ( ImGui::RadioButton(TextureModeToString(TextureMode::NoTexture).c_str(),
                            textureMode == TextureMode::NoTexture) )
      textureMode = TextureMode::NoTexture;
  }

  if ( ImGui::CollapsingHeader("Lighting mode", ImGuiTreeNodeFlags_DefaultOpen) )
  {
    if ( ImGui::RadioButton(LightingModeToString(LightingMode::Diffuse).c_str(),
                            lightingMode == LightingMode::Diffuse) )
      lightingMode = LightingMode::Diffuse;

    if ( ImGui::RadioButton(LightingModeToString(LightingMode::FullBright).c_str(),
                            lightingMode == LightingMode::FullBright) )
      lightingMode = LightingMode::FullBright;
  }
}

} // namespace cqde::compos
