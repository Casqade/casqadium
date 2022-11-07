#include <cqde/types/sequences/TransformInterpolated.hpp>

#include <cqde/components/Tag.hpp>
#include <cqde/components/Transform.hpp>

#include <cqde/types/ui/widgets/StringFilter.hpp>

#include <entt/entity/registry.hpp>

#include <glm/gtx/matrix_interpolation.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <imgui.h>
#include <imgui_bezier.hpp>

#include <optional>


namespace cqde::types
{

void
TransformInterpolated::ui_show(
  const entt::registry& registry,
  const entt::entity entity )
{
  using compos::Tag;
  using compos::Transform;
  using ui::StringFilter;

  Delay::ui_show(registry, entity);

  if ( ImGui::CollapsingHeader("Interpolation", ImGuiTreeNodeFlags_DefaultOpen) )
    ImGui::Bezier("##interpolation", glm::value_ptr(mBezierParams));

  if ( ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen) == false )
    return;

  ImGui::Checkbox("Use world space", &mUseWorldSpace);

  const auto setFromTransform =
  [useWorldSpace = mUseWorldSpace, entity]
  ( const entt::registry& registry, glm::mat4& targetMatrix )
  {
    if ( ImGui::IsWindowAppearing() )
      ImGui::SetKeyboardFocusHere(2);

    static StringFilter entityFilter {"Entity ID"};

    entityFilter.search({}, ImGuiInputTextFlags_AutoSelectAll);

    for ( const auto&& [eReference, cTag, cTransform]
            : registry.view <Tag, Transform> ().each() )
    {
      const auto entityId = cTag.id;

      if ( entityId == null_id )
        continue;

      if ( entityFilter.query(entityId.str()) == false )
        continue;

      if ( ImGui::Selectable(entityId.str().c_str(), false) )
      {
        targetMatrix = GetWorldMatrix(registry, eReference, cTransform);

        if ( useWorldSpace == false )
          targetMatrix = ToLocalSpace(targetMatrix, registry,
                                      entity, cTransform);

        break;
      }
    }
  };

  static std::optional <glm::mat4> transformBuffer {};

  ImGui::Text("Initial");

  ImGui::SameLine();
  if ( ImGui::Button("From Transform##initialFromTransform") )
    ImGui::OpenPopup("##initialFromTransformPopup");

  if ( ImGui::Button("Copy##initialCopy") )
    transformBuffer = mTransform.first;

  ImGui::BeginDisabled(transformBuffer.has_value() == false);

  ImGui::SameLine();
  if ( ImGui::Button("Paste##initialPaste") )
    mTransform.first = transformBuffer.value();

  ImGui::EndDisabled();

  if ( ImGui::BeginTable("##transformInitialMatrix", 4,
                         ImGuiTableFlags_Resizable |
                         ImGuiTableFlags_SizingStretchProp) )
  {
    ImGui::BeginDisabled(true);

    ImGui::TableNextColumn();
    ImGui::SetNextItemWidth(-1.0f);
    ImGui::DragFloat("##transformInitial[0][0]", &mTransform.first[0][0]);

    ImGui::TableNextColumn();
    ImGui::SetNextItemWidth(-1.0f);
    ImGui::DragFloat("##transformInitial[1][0]", &mTransform.first[1][0]);

    ImGui::TableNextColumn();
    ImGui::SetNextItemWidth(-1.0f);
    ImGui::DragFloat("##transformInitial[2][0]", &mTransform.first[2][0]);

    ImGui::TableNextColumn();
    ImGui::SetNextItemWidth(-1.0f);
    ImGui::DragFloat("##transformInitial[3][0]", &mTransform.first[3][0]);

    ImGui::TableNextColumn();
    ImGui::SetNextItemWidth(-1.0f);
    ImGui::DragFloat("##transformInitial[0][1]", &mTransform.first[0][1]);

    ImGui::TableNextColumn();
    ImGui::SetNextItemWidth(-1.0f);
    ImGui::DragFloat("##transformInitial[1][1]", &mTransform.first[1][1]);

    ImGui::TableNextColumn();
    ImGui::SetNextItemWidth(-1.0f);
    ImGui::DragFloat("##transformInitial[2][1]", &mTransform.first[2][1]);

    ImGui::TableNextColumn();
    ImGui::SetNextItemWidth(-1.0f);
    ImGui::DragFloat("##transformInitial[3][1]", &mTransform.first[3][1]);

    ImGui::TableNextColumn();
    ImGui::SetNextItemWidth(-1.0f);
    ImGui::DragFloat("##transformInitial[0][2]", &mTransform.first[0][2]);

    ImGui::TableNextColumn();
    ImGui::SetNextItemWidth(-1.0f);
    ImGui::DragFloat("##transformInitial[1][2]", &mTransform.first[1][2]);

    ImGui::TableNextColumn();
    ImGui::SetNextItemWidth(-1.0f);
    ImGui::DragFloat("##transformInitial[2][2]", &mTransform.first[2][2]);

    ImGui::TableNextColumn();
    ImGui::SetNextItemWidth(-1.0f);
    ImGui::DragFloat("##transformInitial[3][2]", &mTransform.first[3][2]);

    ImGui::TableNextColumn();
    ImGui::SetNextItemWidth(-1.0f);
    ImGui::DragFloat("##transformInitial[0][3]", &mTransform.first[0][3]);

    ImGui::TableNextColumn();
    ImGui::SetNextItemWidth(-1.0f);
    ImGui::DragFloat("##transformInitial[1][3]", &mTransform.first[1][3]);

    ImGui::TableNextColumn();
    ImGui::SetNextItemWidth(-1.0f);
    ImGui::DragFloat("##transformInitial[2][3]", &mTransform.first[2][3]);

    ImGui::TableNextColumn();
    ImGui::SetNextItemWidth(-1.0f);
    ImGui::DragFloat("##transformInitial[3][3]", &mTransform.first[3][3]);

    ImGui::EndDisabled();

    ImGui::EndTable(); // transformInitialMatrix
  }

  ImGui::Text("Target");

  ImGui::SameLine();
  if ( ImGui::Button("From Transform##targetFromTransform") )
    ImGui::OpenPopup("##targetFromTransformPopup");

  if ( ImGui::Button("Copy##targetCopy") )
    transformBuffer = mTransform.second;

  ImGui::BeginDisabled(transformBuffer.has_value() == false);

  ImGui::SameLine();
  if ( ImGui::Button("Paste##targetPaste") )
    mTransform.second = transformBuffer.value();

  ImGui::EndDisabled();

  if ( ImGui::BeginTable("##transformTargetMatrix", 4,
                         ImGuiTableFlags_Resizable |
                         ImGuiTableFlags_SizingStretchProp) )
  {
    ImGui::BeginDisabled(true);

    ImGui::TableNextColumn();
    ImGui::SetNextItemWidth(-1.0f);
    ImGui::DragFloat("##transformTarget[0][0]", &mTransform.second[0][0]);

    ImGui::TableNextColumn();
    ImGui::SetNextItemWidth(-1.0f);
    ImGui::DragFloat("##transformTarget[1][0]", &mTransform.second[1][0]);

    ImGui::TableNextColumn();
    ImGui::SetNextItemWidth(-1.0f);
    ImGui::DragFloat("##transformTarget[2][0]", &mTransform.second[2][0]);

    ImGui::TableNextColumn();
    ImGui::SetNextItemWidth(-1.0f);
    ImGui::DragFloat("##transformTarget[3][0]", &mTransform.second[3][0]);

    ImGui::TableNextColumn();
    ImGui::SetNextItemWidth(-1.0f);
    ImGui::DragFloat("##transformTarget[0][1]", &mTransform.second[0][1]);

    ImGui::TableNextColumn();
    ImGui::SetNextItemWidth(-1.0f);
    ImGui::DragFloat("##transformTarget[1][1]", &mTransform.second[1][1]);

    ImGui::TableNextColumn();
    ImGui::SetNextItemWidth(-1.0f);
    ImGui::DragFloat("##transformTarget[2][1]", &mTransform.second[2][1]);

    ImGui::TableNextColumn();
    ImGui::SetNextItemWidth(-1.0f);
    ImGui::DragFloat("##transformTarget[3][1]", &mTransform.second[3][1]);

    ImGui::TableNextColumn();
    ImGui::SetNextItemWidth(-1.0f);
    ImGui::DragFloat("##transformTarget[0][2]", &mTransform.second[0][2]);

    ImGui::TableNextColumn();
    ImGui::SetNextItemWidth(-1.0f);
    ImGui::DragFloat("##transformTarget[1][2]", &mTransform.second[1][2]);

    ImGui::TableNextColumn();
    ImGui::SetNextItemWidth(-1.0f);
    ImGui::DragFloat("##transformTarget[2][2]", &mTransform.second[2][2]);

    ImGui::TableNextColumn();
    ImGui::SetNextItemWidth(-1.0f);
    ImGui::DragFloat("##transformTarget[3][2]", &mTransform.second[3][2]);

    ImGui::TableNextColumn();
    ImGui::SetNextItemWidth(-1.0f);
    ImGui::DragFloat("##transformTarget[0][3]", &mTransform.second[0][3]);

    ImGui::TableNextColumn();
    ImGui::SetNextItemWidth(-1.0f);
    ImGui::DragFloat("##transformTarget[1][3]", &mTransform.second[1][3]);

    ImGui::TableNextColumn();
    ImGui::SetNextItemWidth(-1.0f);
    ImGui::DragFloat("##transformTarget[2][3]", &mTransform.second[2][3]);

    ImGui::TableNextColumn();
    ImGui::SetNextItemWidth(-1.0f);
    ImGui::DragFloat("##transformTarget[3][3]", &mTransform.second[3][3]);

    ImGui::EndDisabled();

    ImGui::EndTable(); // transformTargetMatrix
  }

  if ( ImGui::BeginPopup("##initialFromTransformPopup") )
  {
    setFromTransform(registry, mTransform.first);
    ImGui::EndPopup(); // initialFromTransformPopup
  }

  if ( ImGui::BeginPopup("##targetFromTransformPopup") )
  {
    setFromTransform(registry, mTransform.second);
    ImGui::EndPopup(); // targetFromTransformPopup
  }
}

} // namespace cqde::types
