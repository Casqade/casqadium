#include <cqde/types/sequences/TransformManipulate.hpp>

#include <cqde/components/Tag.hpp>
#include <cqde/conversion/enum_strings.hpp>
#include <cqde/types/ui/widgets/StringFilter.hpp>

#include <entt/entity/registry.hpp>

#include <glm/gtc/type_ptr.hpp>

#include <imgui.h>

#include <cqde/logger.hpp>


namespace cqde::types
{

void
TransformManipulate::ui_show(
  const entt::registry& registry,
  const entt::entity entity )
{
  using compos::Tag;
  using compos::TextureTint;
  using ui::StringFilter;

  const auto flags = ImGuiSliderFlags_AlwaysClamp |
                     ImGuiSliderFlags_NoRoundToFormat;

  mTime.ui_show();

  if ( ImGui::CollapsingHeader("Operation", ImGuiTreeNodeFlags_DefaultOpen) )
  {
    ImGui::Checkbox("Relative transform", &mTransformRelative);

    if ( ImGui::BeginCombo("##operationType",
                           TransformTypeToString(mOperation).c_str()) )
    {
      if ( ImGui::Selectable(TransformTypeToString(TransformType::Translate).c_str(),
                             mOperation == TransformType::Translate) )
        mOperation = TransformType::Translate;

      if ( ImGui::Selectable(TransformTypeToString(TransformType::Rotate).c_str(),
                             mOperation == TransformType::Rotate) )
        mOperation = TransformType::Rotate;

      if ( ImGui::Selectable(TransformTypeToString(TransformType::Scale).c_str(),
                             mOperation == TransformType::Scale) )
        mOperation = TransformType::Scale;

      ImGui::EndCombo();
    }
  }

  if ( ImGui::CollapsingHeader("Axis", ImGuiTreeNodeFlags_DefaultOpen) )
  {
    ImGui::Checkbox("In world space##useWorldSpace", &mUseWorldSpace);

    ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);

    ImGui::DragFloat("##axisX", &mAxis.x, 0.01f,
                     -1.0f, 1.0f, "X: %.2f", flags);

    ImGui::DragFloat("##axisY", &mAxis.y, 0.01f,
                     -1.0f, 1.0f, "Y: %.2f", flags);

    ImGui::DragFloat("##axisZ", &mAxis.z, 0.01f,
                     -1.0f, 1.0f, "Z: %.2f", flags);

    ImGui::PopItemWidth();
  }

  if ( ImGui::CollapsingHeader("Function", ImGuiTreeNodeFlags_DefaultOpen) )
  {
    if ( ImGui::BeginCombo("##functionType",
                           MathFunctionTypeToString(mFunction).c_str()) )
    {
      if ( ImGui::Selectable(MathFunctionTypeToString(MathFunctionType::Constant).c_str(),
                             mFunction == MathFunctionType::Constant) )
        mFunction = MathFunctionType::Constant;

      if ( ImGui::Selectable(MathFunctionTypeToString(MathFunctionType::Linear).c_str(),
                             mFunction == MathFunctionType::Linear) )
        mFunction = MathFunctionType::Linear;

      if ( ImGui::Selectable(MathFunctionTypeToString(MathFunctionType::Sine).c_str(),
                             mFunction == MathFunctionType::Sine) )
        mFunction = MathFunctionType::Sine;

      if ( ImGui::Selectable(MathFunctionTypeToString(MathFunctionType::Cosine).c_str(),
                             mFunction == MathFunctionType::Cosine) )
        mFunction = MathFunctionType::Cosine;

      if ( ImGui::Selectable(MathFunctionTypeToString(MathFunctionType::Pulse).c_str(),
                             mFunction == MathFunctionType::Pulse) )
        mFunction = MathFunctionType::Pulse;

      if ( ImGui::Selectable(MathFunctionTypeToString(MathFunctionType::Triangle).c_str(),
                             mFunction == MathFunctionType::Triangle) )
        mFunction = MathFunctionType::Triangle;

      if ( ImGui::Selectable(MathFunctionTypeToString(MathFunctionType::Sawtooth).c_str(),
                             mFunction == MathFunctionType::Sawtooth) )
        mFunction = MathFunctionType::Sawtooth;

      ImGui::EndCombo();
    }

    const double zero {};
    const double min = std::numeric_limits <double>::min();
    const double max = std::numeric_limits <double>::max();

    ImGui::DragScalar("##amplitude", ImGuiDataType_Double, &mAmplitude,
                      0.01f, &zero, &max, "Amplitude: %.2f", flags);

    ImGui::DragScalar("##period", ImGuiDataType_Double, &mPeriod,
                      0.01f, &min, &max, "Period: %.2f", flags);

    ImGui::DragScalar("##phase", ImGuiDataType_Double, &mPhase,
                      0.01f, &zero, &zero, "Phase: %.2f", flags);

    ImGui::DragScalar("##offset", ImGuiDataType_Double, &mOffset,
                      0.01f, &zero, &zero, "Offset: %.2f", flags);
  }
}

} // namespace cqde::types
