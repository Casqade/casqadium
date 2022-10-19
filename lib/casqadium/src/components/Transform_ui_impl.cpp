#include <cqde/components/Transform.hpp>
#include <cqde/components/SceneNode.hpp>

#include <entt/entity/registry.hpp>

#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/compatibility.hpp>

#include <imgui.h>


namespace cqde::compos
{

void
Transform::ui_edit_props(
  const entt::entity entity,
  const entt::registry& registry )
{
  static enum class OrientationMode
  {
    Quat,
    Euler,
    Matrix,

  } orientationMode {};

  static bool showInWorldSpace {};

  if ( registry.all_of <SceneNode> (entity) == false )
    showInWorldSpace = false;

  else
  {
    if ( ImGui::CollapsingHeader("Coordinate space", ImGuiTreeNodeFlags_DefaultOpen) )
    {
      if ( ImGui::RadioButton("Local space", showInWorldSpace == false) )
        showInWorldSpace = false;

      if ( ImGui::RadioButton("World space", showInWorldSpace == true) )
        showInWorldSpace = true;
    }
  }

  const auto flags = ImGuiSliderFlags_NoRoundToFormat |
                     ImGuiSliderFlags_AlwaysClamp;

  if ( ImGui::BeginTabBar("Transform") == false )
    return;

  if ( ImGui::BeginTabItem("Translation") )
  {
    auto translationBuffer = translation;

    if ( showInWorldSpace == true )
      translationBuffer = ToWorldSpace(translation, registry,
                                       entity, *this);

    ImGui::AlignTextToFramePadding();
    ImGui::Text("X");

    ImGui::SameLine();
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
    ImGui::DragFloat("##translationX", &translationBuffer.x,
                     0.01f, 0.0f, 0.0f, "%.3f", flags);

    ImGui::AlignTextToFramePadding();
    ImGui::Text("Y");

    ImGui::SameLine();
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
    ImGui::DragFloat("##translationY", &translationBuffer.y,
                     0.01f, 0.0f, 0.0f, "%.3f", flags);

    ImGui::AlignTextToFramePadding();
    ImGui::Text("Z");

    ImGui::SameLine();
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
    ImGui::DragFloat("##translationZ", &translationBuffer.z,
                     0.01f, 0.0f, 0.0f, "%.3f", flags);

    if ( ImGui::Button("Reset##translationReset") )
      translationBuffer = glm::vec3{};

    if ( translationBuffer != translation )
    {
      if ( showInWorldSpace == false )
        translation = translationBuffer;
      else
        SetTranslationWorld(translationBuffer, registry,
                            entity, *this);
    }

    ImGui::EndTabItem(); // Translation
  }

  if ( ImGui::BeginTabItem("Orientation") )
  {
    auto orientationBuffer = orientation;

    if ( showInWorldSpace == true )
      orientationBuffer = ToWorldSpace(orientation, registry,
                                       entity, *this);

    if ( ImGui::RadioButton("Quaternion",
                            orientationMode == OrientationMode::Quat) )
      orientationMode = OrientationMode::Quat;

    if ( ImGui::RadioButton("Euler",
                            orientationMode == OrientationMode::Euler) )
      orientationMode = OrientationMode::Euler;

    if ( ImGui::RadioButton("Matrix",
                            orientationMode == OrientationMode::Matrix) )
      orientationMode = OrientationMode::Matrix;

    switch (orientationMode)
    {
      case OrientationMode::Quat:
      {
        ImGui::AlignTextToFramePadding();
        ImGui::Text("X");

        ImGui::SameLine();
        ImGui::BeginDisabled(true);
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        ImGui::DragFloat( "##orientationX", &orientationBuffer.x,
                          glm::radians(0.1f), 0.0f, 1.0f, "%.3f",
                          flags);
        ImGui::EndDisabled();

        ImGui::AlignTextToFramePadding();
        ImGui::Text("Y");

        ImGui::SameLine();
        ImGui::BeginDisabled(true);
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        ImGui::DragFloat( "##orientationY", &orientationBuffer.y,
                          glm::radians(0.1f), 0.0f, 1.0f, "%.3f",
                          flags);
        ImGui::EndDisabled();

        ImGui::AlignTextToFramePadding();
        ImGui::Text("Z");

        ImGui::SameLine();
        ImGui::BeginDisabled(true);
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        ImGui::DragFloat( "##orientationZ", &orientationBuffer.z,
                          glm::radians(0.1f), 0.0f, 1.0f, "%.3f",
                          flags);
        ImGui::EndDisabled();

        ImGui::AlignTextToFramePadding();
        ImGui::Text("W");

        ImGui::SameLine();
        ImGui::BeginDisabled(true);
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        ImGui::DragFloat( "##orientationW", &orientationBuffer.w,
                          glm::radians(0.1f), 0.0f, 1.0f, "%.3f",
                          flags);
        ImGui::EndDisabled();

        break;
      }

      case OrientationMode::Euler:
      {
        auto orientationEuler = glm::degrees(glm::eulerAngles(orientationBuffer));

        ImGui::AlignTextToFramePadding();
        ImGui::Text("X");

        ImGui::SameLine();
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        ImGui::DragFloat( "##orientationX", &orientationEuler.x,
                          0.1f, -180.0f, 180.0f, "%.1f°", flags);

        ImGui::AlignTextToFramePadding();
        ImGui::Text("Y");

        ImGui::SameLine();
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        ImGui::DragFloat( "##orientationY", &orientationEuler.y,
                          0.1f, -180.0f, 180.0f, "%.1f°", flags);

        ImGui::AlignTextToFramePadding();
        ImGui::Text("Z");

        ImGui::SameLine();
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        ImGui::DragFloat( "##orientationZ", &orientationEuler.z,
                          0.1f, -180.0f, 180.0f, "%.1f°", flags);

        orientationBuffer = glm::radians(orientationEuler);

        break;
      }

      case OrientationMode::Matrix:
      {
        auto orientationMat = glm::toMat4(orientationBuffer);

        auto right = glm::column(orientationMat, 0);
        auto up = glm::column(orientationMat, 1);
        auto front = glm::column(orientationMat, 2);

        if ( ImGui::BeginTable("##orientationMatrix", 4,
                               ImGuiTableFlags_Resizable |
                               ImGuiTableFlags_SizingStretchProp) )
        {
          ImGui::TableSetupColumn("##dimension", ImGuiTableColumnFlags_NoHeaderLabel);
          ImGui::TableSetupColumn("Right", ImGuiTableColumnFlags_NoHeaderWidth);
          ImGui::TableSetupColumn("Up", ImGuiTableColumnFlags_NoHeaderWidth);
          ImGui::TableSetupColumn("Front", ImGuiTableColumnFlags_NoHeaderWidth);

          ImGui::TableHeadersRow();

          ImGui::TableNextColumn();
          ImGui::Text("X");

          ImGui::BeginDisabled(true);

          ImGui::TableNextColumn();
          ImGui::SetNextItemWidth(-1.0f);
          ImGui::DragFloat("##orientationRightX", &right.x);

          ImGui::TableNextColumn();
          ImGui::SetNextItemWidth(-1.0f);
          ImGui::DragFloat("##orientationUpX", &up.x);

          ImGui::TableNextColumn();
          ImGui::SetNextItemWidth(-1.0f);
          ImGui::DragFloat("##orientationFrontX", &front.x);

          ImGui::EndDisabled();

          ImGui::TableNextColumn();
          ImGui::Text("Y");

          ImGui::BeginDisabled(true);

          ImGui::TableNextColumn();
          ImGui::SetNextItemWidth(-1.0f);
          ImGui::DragFloat("##orientationRightY", &right.y);

          ImGui::TableNextColumn();
          ImGui::SetNextItemWidth(-1.0f);
          ImGui::DragFloat("##orientationUpY", &up.y);

          ImGui::TableNextColumn();
          ImGui::SetNextItemWidth(-1.0f);
          ImGui::DragFloat("##orientationFrontY", &front.y);

          ImGui::EndDisabled();

          ImGui::TableNextColumn();
          ImGui::Text("Z");

          ImGui::BeginDisabled(true);

          ImGui::TableNextColumn();
          ImGui::SetNextItemWidth(-1.0f);
          ImGui::DragFloat("##orientationRightZ", &right.z);

          ImGui::TableNextColumn();
          ImGui::SetNextItemWidth(-1.0f);
          ImGui::DragFloat("##orientationUpZ", &up.z);

          ImGui::TableNextColumn();
          ImGui::SetNextItemWidth(-1.0f);
          ImGui::DragFloat("##orientationFrontZ", &front.z);

          ImGui::EndDisabled();

          ImGui::EndTable(); // orientationMatrix
        }

        break;
      }
    }

    if ( ImGui::Button("Reset##orientationReset") )
      orientationBuffer = glm::vec3{};

    if ( orientationBuffer != orientation &&
         glm::all(glm::isfinite(glm::eulerAngles(orientationBuffer))) == true )
    {
      if ( showInWorldSpace == false )
        orientation = glm::normalize(orientationBuffer);
      else
        SetOrientationWorld(orientationBuffer, registry,
                            entity, *this);
    }

    ImGui::EndTabItem(); // Orientation
  }

  if ( ImGui::BeginTabItem("Scale") )
  {
    ImGui::Text("Local");

    ImGui::AlignTextToFramePadding();
    ImGui::Text("X");

    ImGui::SameLine();
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
    ImGui::DragFloat("##scaleX", &scale.x, 0.01f,
                     0.0f, 0.0f, "%.2f", flags);

    ImGui::AlignTextToFramePadding();
    ImGui::Text("Y");

    ImGui::SameLine();
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
    ImGui::DragFloat("##scaleY", &scale.y, 0.01f,
                     0.0f, 0.0f, "%.2f", flags);

    ImGui::AlignTextToFramePadding();
    ImGui::Text("Z");

    ImGui::SameLine();
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
    ImGui::DragFloat("##scaleZ", &scale.z, 0.01f,
                     0.0f, 0.0f, "%.2f", flags);

    if ( ImGui::Button("Reset##scaleReset") )
      scale = glm::vec3{1.0f};

    ImGui::Spacing();

    ImGui::Text("World");

    ImGui::AlignTextToFramePadding();
    ImGui::Text("X");

    ImGui::SameLine();
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
    ImGui::DragFloat("##scaleWorldX", &scaleWorld.x, 0.01f,
                     0.0f, 0.0f, "%.2f", flags);

    ImGui::AlignTextToFramePadding();
    ImGui::Text("Y");

    ImGui::SameLine();
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
    ImGui::DragFloat("##scaleWorldY", &scaleWorld.y, 0.01f,
                     0.0f, 0.0f, "%.2f", flags);

    ImGui::AlignTextToFramePadding();
    ImGui::Text("Z");

    ImGui::SameLine();
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
    ImGui::DragFloat("##scaleWorldZ", &scaleWorld.z, 0.01f,
                     0.0f, 0.0f, "%.2f", flags);

    if ( ImGui::Button("Reset##scaleWorldReset") )
      scaleWorld = glm::vec3{1.0f};

    ImGui::EndTabItem(); // Scale
  }

  ImGui::EndTabBar(); // Transform
}

} // namespace cqde
