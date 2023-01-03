#include <cqde/components/audio/AudioDrivenTransform.hpp>

#include <cqde/conversion/enum_strings.hpp>
#include <cqde/types/ui/widgets/StringFilter.hpp>
#include <cqde/types/EntityManager.hpp>

#include <entt/entity/registry.hpp>

#include <imgui.h>


namespace cqde::compos
{

void
AudioDrivenTransform::ui_edit_props(
  const entt::entity entity,
  const entt::registry& registry )
{
  using ui::StringFilter;
  using types::EntityManager;

  const auto flags = ImGuiSliderFlags_NoRoundToFormat |
                     ImGuiSliderFlags_AlwaysClamp;

  if ( ImGui::CollapsingHeader("Source entity", ImGuiTreeNodeFlags_DefaultOpen) )
  {
    const auto entityList = registry.ctx().at <EntityManager> ().entities();

    static StringFilter entityFilter {"Entity ID"};

    if ( ImGui::BeginCombo("##entitySourceId", sourceEntity.id.str().c_str()) )
    {
      if ( ImGui::IsWindowAppearing() )
        ImGui::SetKeyboardFocusHere(2);

      entityFilter.search({}, ImGuiInputTextFlags_AutoSelectAll);

      for ( const auto& entityId : entityList )
      {
        if ( entityFilter.query(entityId.str()) == false )
          continue;

        const bool selected = (sourceEntity == entityId);

        if ( ImGui::Selectable(entityId.str().c_str(), selected) )
          sourceEntity = entityId;
      }
      ImGui::EndCombo();
    }
  }

  if ( ImGui::CollapsingHeader("Operation", ImGuiTreeNodeFlags_DefaultOpen) )
  {
    if ( ImGui::BeginCombo("##operationType",
                           TransformTypeToString(operation).c_str()) )
    {
      if ( ImGui::Selectable(TransformTypeToString(TransformType::Translate).c_str(),
                             operation == TransformType::Translate) )
        operation = TransformType::Translate;

      if ( ImGui::Selectable(TransformTypeToString(TransformType::Rotate).c_str(),
                             operation == TransformType::Rotate) )
        operation = TransformType::Rotate;

      if ( ImGui::Selectable(TransformTypeToString(TransformType::Scale).c_str(),
                             operation == TransformType::Scale) )
        operation = TransformType::Scale;

      ImGui::EndCombo();
    }
  }

  if ( ImGui::CollapsingHeader("Axis", ImGuiTreeNodeFlags_DefaultOpen) )
  {
    ImGui::Checkbox("In world space##useWorldSpace", &useWorldSpace);

    ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);

    ImGui::DragFloat("##axisX", &axis.x, 0.01f,
                     -1.0f, 1.0f, "X: %.2f", flags);

    ImGui::DragFloat("##axisY", &axis.y, 0.01f,
                     -1.0f, 1.0f, "Y: %.2f", flags);

    ImGui::DragFloat("##axisZ", &axis.z, 0.01f,
                     -1.0f, 1.0f, "Z: %.2f", flags);

    ImGui::PopItemWidth();

    if ( ImGui::Button("Reset##forceReset") )
      axis = glm::vec3{};
  }

  if ( ImGui::CollapsingHeader("Magnitude", ImGuiTreeNodeFlags_DefaultOpen) )
    ImGui::DragFloat("##magnitude", &magnitude, 0.1f,
                     0.0f, 0.0f,
                     "%.1f", flags);

  if ( ImGui::CollapsingHeader("Offset", ImGuiTreeNodeFlags_DefaultOpen) )
    ImGui::DragFloat("##offset", &offset, 0.1f,
                     0.0f, 0.0f,
                     "%.1f", flags);

  if ( ImGui::CollapsingHeader("Damp", ImGuiTreeNodeFlags_DefaultOpen) )
    ImGui::DragFloat("##damp", &damp, 0.01f,
                     0.0f, 0.99,
                     "%.2f", flags);

  if ( ImGui::CollapsingHeader("Ignore bus volume", ImGuiTreeNodeFlags_DefaultOpen) )
    ImGui::Checkbox("##ignoreBusVolume", &ignoreBusVolume);
}

} // namespace cqde
