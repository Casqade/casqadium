#include <cqde/components/Transform.hpp>

#include <imgui.h>


namespace cqde::compos
{

void
Transform::ui_edit_props(
  const entt::entity,
  const entt::registry& )
{
  static bool orientationAsQuat {};

  bool orientationChanged {};

  if ( ImGui::CollapsingHeader("Translation", ImGuiTreeNodeFlags_DefaultOpen) )
  {
    ImGui::AlignTextToFramePadding();
    ImGui::Text("X");

    ImGui::SameLine();
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
    ImGui::DragFloat("##translationX", &translation.x,
                     0.01f, 0.0f, 0.0f, "%.3f",
                     ImGuiSliderFlags_NoRoundToFormat);

    ImGui::AlignTextToFramePadding();
    ImGui::Text("Y");

    ImGui::SameLine();
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
    ImGui::DragFloat("##translationY", &translation.y,
                     0.01f, 0.0f, 0.0f, "%.3f",
                     ImGuiSliderFlags_NoRoundToFormat);

    ImGui::AlignTextToFramePadding();
    ImGui::Text("Z");

    ImGui::SameLine();
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
    ImGui::DragFloat("##translationZ", &translation.z,
                     0.01f, 0.0f, 0.0f, "%.3f",
                     ImGuiSliderFlags_NoRoundToFormat);

    if ( ImGui::Button("Reset##translationReset") )
      translation = glm::vec3{};
  }

  if ( ImGui::CollapsingHeader("Orientation", ImGuiTreeNodeFlags_DefaultOpen) )
  {
    if ( orientationAsQuat == true )
    {
      if ( ImGui::Button("Quaternion") )
        orientationAsQuat = false;
    }
    else if ( ImGui::Button("XYZ Euler") )
      orientationAsQuat = true;

    if ( orientationAsQuat == true )
    {
      ImGui::AlignTextToFramePadding();
      ImGui::Text("X");

      ImGui::SameLine();
      ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
      if ( ImGui::DragFloat("##orientationX", &orientation.x,
                            glm::radians(0.1f), 0.0f, 0.0f, "%.3f",
                            ImGuiSliderFlags_NoRoundToFormat | ImGuiSliderFlags_AlwaysClamp) )
        orientationChanged = true;

      ImGui::AlignTextToFramePadding();
      ImGui::Text("Y");

      ImGui::SameLine();
      ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
      if ( ImGui::DragFloat("##orientationY", &orientation.y,
                            glm::radians(0.1f), 0.0f, 0.0f, "%.3f",
                            ImGuiSliderFlags_NoRoundToFormat | ImGuiSliderFlags_AlwaysClamp) )
        orientationChanged = true;

      ImGui::AlignTextToFramePadding();
      ImGui::Text("Z");

      ImGui::SameLine();
      ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
      if ( ImGui::DragFloat("##orientationZ", &orientation.z,
                            glm::radians(0.1f), 0.0f, 0.0f, "%.3f",
                            ImGuiSliderFlags_NoRoundToFormat | ImGuiSliderFlags_AlwaysClamp) )
        orientationChanged = true;

      ImGui::AlignTextToFramePadding();
      ImGui::Text("W");

      ImGui::SameLine();
      ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
      if ( ImGui::DragFloat("##orientationW", &orientation.w,
                            glm::radians(0.1f), 0.0f, 0.0f, "%.3f",
                            ImGuiSliderFlags_NoRoundToFormat | ImGuiSliderFlags_AlwaysClamp) )
        orientationChanged = true;


      if ( orientationChanged == true )
        orientation = glm::normalize(orientation);
    }
    else
    {
      glm::vec3 orientationEuler = glm::degrees(glm::eulerAngles(orientation));

      ImGui::AlignTextToFramePadding();
      ImGui::Text("X");

      ImGui::SameLine();
      ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
      if ( ImGui::DragFloat("##orientationX", &orientationEuler.x,
                            0.1f, -180.0f, 180.0f, "%.1f°",
                            ImGuiSliderFlags_NoRoundToFormat | ImGuiSliderFlags_AlwaysClamp) )
        orientationChanged = true;

      ImGui::AlignTextToFramePadding();
      ImGui::Text("Y");

      ImGui::SameLine();
      ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
      if ( ImGui::DragFloat("##orientationY", &orientationEuler.y,
                            0.1f, -180.0f, 180.0f, "%.1f°",
                            ImGuiSliderFlags_NoRoundToFormat | ImGuiSliderFlags_AlwaysClamp) )
        orientationChanged = true;

      ImGui::AlignTextToFramePadding();
      ImGui::Text("Z");

      ImGui::SameLine();
      ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
      if ( ImGui::DragFloat("##orientationZ", &orientationEuler.z,
                            0.1f, -180.0f, 180.0f, "%.1f°",
                            ImGuiSliderFlags_NoRoundToFormat | ImGuiSliderFlags_AlwaysClamp) )
        orientationChanged = true;

      if ( orientationChanged )
        orientation = glm::normalize(glm::quat{glm::radians(orientationEuler)});
    }

    if ( ImGui::Button("Reset##orientationReset") )
      orientation = glm::vec3{};
  }

  if ( ImGui::CollapsingHeader("Scale", ImGuiTreeNodeFlags_DefaultOpen) )
  {
    ImGui::AlignTextToFramePadding();
    ImGui::Text("X");

    ImGui::SameLine();
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
    ImGui::DragFloat("##scaleX", &scale.x,
                     0.01f, 0.0f, 0.0f, "%.2f",
                     ImGuiSliderFlags_NoRoundToFormat);

    ImGui::AlignTextToFramePadding();
    ImGui::Text("Y");

    ImGui::SameLine();
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
    ImGui::DragFloat("##scaleY", &scale.y,
                     0.01f, 0.0f, 0.0f, "%.2f",
                     ImGuiSliderFlags_NoRoundToFormat);

    ImGui::AlignTextToFramePadding();
    ImGui::Text("Z");

    ImGui::SameLine();
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
    ImGui::DragFloat("##scaleZ", &scale.z,
                     0.01f, 0.0f, 0.0f, "%.2f",
                     ImGuiSliderFlags_NoRoundToFormat);

    if ( ImGui::Button("Reset##scaleReset") )
      scale = glm::vec3{1.0f};
  }

  if ( ImGui::CollapsingHeader("Scale (world)", ImGuiTreeNodeFlags_DefaultOpen) )
  {
    ImGui::AlignTextToFramePadding();
    ImGui::Text("X");

    ImGui::SameLine();
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
    ImGui::DragFloat("##scaleWorldX", &scaleWorld.x,
                     0.01f, 0.0f, 0.0f, "%.2f",
                     ImGuiSliderFlags_NoRoundToFormat);

    ImGui::AlignTextToFramePadding();
    ImGui::Text("Y");

    ImGui::SameLine();
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
    ImGui::DragFloat("##scaleWorldY", &scaleWorld.y,
                     0.01f, 0.0f, 0.0f, "%.2f",
                     ImGuiSliderFlags_NoRoundToFormat);

    ImGui::AlignTextToFramePadding();
    ImGui::Text("Z");

    ImGui::SameLine();
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
    ImGui::DragFloat("##scaleWorldZ", &scaleWorld.z,
                     0.01f, 0.0f, 0.0f, "%.2f",
                     ImGuiSliderFlags_NoRoundToFormat);

    if ( ImGui::Button("Reset##scaleWorldReset") )
      scaleWorld = glm::vec3{1.0f};
  }
}

} // namespace cqde
