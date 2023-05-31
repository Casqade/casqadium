#include <cqde/types/sequences/CameraFovInterpolated.hpp>

#include <entt/entity/registry.hpp>

#include <glm/gtx/matrix_interpolation.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <imgui_bezier.hpp>

#include <optional>


namespace cqde::types
{

void
CameraFovInterpolated::ui_show(
  const entt::registry& registry,
  const entt::entity entity )
{
  mTime.ui_show();

  if ( ImGui::CollapsingHeader("Interpolation curve", ImGuiTreeNodeFlags_DefaultOpen) )
  {
    glm::vec4 curve {mSpline.p0(), mSpline.p1()};

    ImGui::Bezier("##interpolationCurve", glm::value_ptr(curve));
    mSpline.setPoints({curve}, {curve.z, curve.w});
  }

  if ( ImGui::CollapsingHeader("FoV", ImGuiTreeNodeFlags_DefaultOpen) == false )
    return;


  const auto editFov =
  [] ( float& fov, const std::string& widgetId )
  {
    const auto flags =
      ImGuiSliderFlags_AlwaysClamp |
      ImGuiSliderFlags_NoRoundToFormat;

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

} // namespace cqde::types
