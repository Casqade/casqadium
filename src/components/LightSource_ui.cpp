#include <cqde/components/LightSource.hpp>

#include <entt/entity/registry.hpp>

#include <imgui.h>


namespace cqde::compos
{

void
LightSource::ui_edit_props(
  const entt::entity,
  const entt::registry& registry )
{
  const auto flags = ImGuiSliderFlags_AlwaysClamp |
                     ImGuiSliderFlags_NoRoundToFormat;

  if ( ImGui::CollapsingHeader("Type", ImGuiTreeNodeFlags_DefaultOpen) )
  {
    if ( ImGui::RadioButton(LightSourceTypeToString(Type::Ambient).c_str(),
                            type == Type::Ambient) )
      type = Type::Ambient;

    if ( ImGui::RadioButton(LightSourceTypeToString(Type::Positional).c_str(),
                            type == Type::Positional) )
      type = Type::Positional;

    if ( ImGui::RadioButton(LightSourceTypeToString(Type::Directional).c_str(),
                            type == Type::Directional) )
      type = Type::Directional;
  }

  if ( ImGui::CollapsingHeader("Color", ImGuiTreeNodeFlags_DefaultOpen) )
  {
    float colorBuffer[4]
    {
      color.r / 255.0f,
      color.g / 255.0f,
      color.b / 255.0f,
      color.a / 255.0f,
    };

    if ( ImGui::ColorPicker4("##color", colorBuffer) )
      color = olc::PixelF(colorBuffer[0], colorBuffer[1],
                          colorBuffer[2], colorBuffer[3]);
  }

  if ( ImGui::CollapsingHeader("Radius", ImGuiTreeNodeFlags_DefaultOpen) )
    ImGui::DragFloat("##radius", &radius, 0.1f,
                     0.0f, std::numeric_limits <float>::max(),
                     "%.1f", flags);

  if ( ImGui::CollapsingHeader("Attenuation", ImGuiTreeNodeFlags_DefaultOpen) )
  {
    ImGui::Text("Constant");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
    ImGui::DragFloat("##attenuationConstant",
                     &attenuationConstant,
                     0.01f, 0.0f, 1.0f,
                     "%.2f", flags);

    ImGui::Text("Linear");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
    ImGui::DragFloat("##attenuationLinear",
                     &attenuationLinear,
                     0.001f, 0.0f, 1.0f,
                     "%.2f", flags);

    ImGui::Text("Quadratic");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
    ImGui::DragFloat("##attenuationQuadratic",
                     &attenuationQuadratic,
                     0.0001f, 0.0f, 1.0f,
                     "%.2f", flags);
  }
}

} // namespace cqde::compos
