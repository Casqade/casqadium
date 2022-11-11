#include <demo/sequences/CameraFovInterpolated.hpp>

#include <entt/entity/registry.hpp>

#include <glm/gtx/matrix_interpolation.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <imgui.h>
#include <imgui_bezier.hpp>

#include <optional>


namespace demo
{

void
CameraFovInterpolated::ui_show(
  const entt::registry& registry,
  const entt::entity entity )
{
  using cqde::compos::Camera;

  const auto flags = ImGuiSliderFlags_AlwaysClamp |
                     ImGuiSliderFlags_NoRoundToFormat;

  Delay::ui_show(registry, entity);

  if ( ImGui::CollapsingHeader("Interpolation", ImGuiTreeNodeFlags_DefaultOpen) )
    ImGui::Bezier("##interpolation", glm::value_ptr(mBezierParams));

  if ( ImGui::CollapsingHeader("FoV", ImGuiTreeNodeFlags_DefaultOpen) == false )
    return;


  const auto editFov =
  [] ( float& fov, const std::string& widgetId )
  {
    static bool fovAsRadians {};

    if ( fovAsRadians == false )
    {
      float fovDegrees = glm::degrees(fov);

      if ( ImGui::DragFloat(widgetId.c_str(), &fovDegrees,
                            1.0f, std::numeric_limits <float>::min(),
                            180.0f, "%.2f deg", flags) )
        fov = glm::radians(fovDegrees);
    }
    else
      ImGui::DragFloat(widgetId.c_str(), &fov,
                       glm::radians(1.0f),
                       std::numeric_limits <float>::min(),
                       glm::pi <float> (), "%.3f rad", flags);

    ImGui::SameLine();
    if ( fovAsRadians == true )
    {
      if ( ImGui::Button("rad") )
        fovAsRadians = false;
    }
    else if ( ImGui::Button("deg") )
      fovAsRadians = true;
  };

  ImGui::Checkbox("Init from camera FoV value", &mInitFromCurrentFov);

  ImGui::Text("Initial");
  editFov(mFov.first, "##fovInitial");

  ImGui::Text("Target");
  editFov(mFov.second, "##forTarget");

}

} // namespace demo
