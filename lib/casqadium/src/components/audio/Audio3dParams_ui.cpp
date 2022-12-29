#include <cqde/components/audio/Audio3dParams.hpp>

#include <cqde/conversion/soloud_string.hpp>

#include <entt/entity/registry.hpp>

#include <imgui.h>


namespace cqde::compos
{

void
Audio3dParams::ui_edit_props(
  const entt::entity entity,
  const entt::registry& registry)
{
  const auto flags = ImGuiSliderFlags_AlwaysClamp |
                     ImGuiSliderFlags_NoRoundToFormat;

  if ( ImGui::CollapsingHeader("Attenuation model", ImGuiTreeNodeFlags_DefaultOpen) )
  {
    SoLoudAttenuationModelStringConverter modelConverter {};

    if ( ImGui::BeginCombo("##attenuationModel",
                           modelConverter.toString(attenuation.model).c_str()) )
    {
      if ( ImGui::Selectable(modelConverter.toString(AttenuationModel::NO_ATTENUATION).c_str(),
                             attenuation.model == AttenuationModel::NO_ATTENUATION) )
        attenuation.model = AttenuationModel::NO_ATTENUATION;

      if ( ImGui::Selectable(modelConverter.toString(AttenuationModel::INVERSE_DISTANCE).c_str(),
                             attenuation.model == AttenuationModel::INVERSE_DISTANCE) )
        attenuation.model = AttenuationModel::INVERSE_DISTANCE;

      if ( ImGui::Selectable(modelConverter.toString(AttenuationModel::LINEAR_DISTANCE).c_str(),
                             attenuation.model == AttenuationModel::LINEAR_DISTANCE) )
        attenuation.model = AttenuationModel::LINEAR_DISTANCE;

      if ( ImGui::Selectable(modelConverter.toString(AttenuationModel::EXPONENTIAL_DISTANCE).c_str(),
                             attenuation.model == AttenuationModel::EXPONENTIAL_DISTANCE) )
        attenuation.model = AttenuationModel::EXPONENTIAL_DISTANCE;

      ImGui::EndCombo();
    }
  }

  if ( ImGui::CollapsingHeader("Attenuation rolloff", ImGuiTreeNodeFlags_DefaultOpen) )
    ImGui::DragFloat("##attenuationRolloff", &attenuation.rolloff, 0.01f,
                     0.0f, 0.0f, "%.2f", flags);

  if ( ImGui::CollapsingHeader("Attenuation distance", ImGuiTreeNodeFlags_DefaultOpen) )
  {
    ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);

    ImGui::DragFloat("##attenuationDistanceMin", &attenuation.distance.min,
                      0.01f, 0.0f, attenuation.distance.max,
                      "Min %.2f",flags);

    ImGui::DragFloat("##attenuationDistanceMax", &attenuation.distance.max,
                      0.01f, attenuation.distance.min,
                     std::numeric_limits <float>::max(),
                     "Max %.2f", flags);

    ImGui::PopItemWidth();
  }

  if ( ImGui::CollapsingHeader("Doppler factor", ImGuiTreeNodeFlags_DefaultOpen) )
    ImGui::DragFloat("##dopplerFactor", &dopplerFactor, 0.01f,
                     0.0f, 0.0f, "%.2f", flags);
}

} // namespace cqde::compos
