#include <cqde/types/sequences/TextureTintInterpolated.hpp>

#include <cqde/components/Tag.hpp>
#include <cqde/components/TextureTint.hpp>

#include <cqde/types/ui/widgets/StringFilter.hpp>

#include <entt/entity/registry.hpp>

#include <glm/gtc/type_ptr.hpp>

#include <imgui.h>
#include <imgui_bezier.hpp>

#include <optional>

#include <cqde/util/logger.hpp>

namespace cqde::types
{

void
TextureTintInterpolated::ui_show(
  const entt::registry& registry,
  const entt::entity entity )
{
  using compos::Tag;
  using compos::TextureTint;
  using ui::StringFilter;

  mTime.ui_show();

  if ( ImGui::CollapsingHeader("Interpolation curve", ImGuiTreeNodeFlags_DefaultOpen) )
  {
    glm::vec4 curve {mSpline.p0(), mSpline.p1()};

    ImGui::Bezier("##interpolationCurve", glm::value_ptr(curve));
    mSpline.setPoints({curve}, {curve.z, curve.w});
  }

  if ( ImGui::CollapsingHeader("Tint", ImGuiTreeNodeFlags_DefaultOpen) == false )
    return;

  ImGui::Checkbox("Init from TextureTint", &mInitFromTextureTint);

  const auto setFromTextureTint =
  [entity]
  ( const entt::registry& registry, glm::vec4& targetTint )
  {
    if ( ImGui::IsWindowAppearing() )
      ImGui::SetKeyboardFocusHere(2);

    static StringFilter entityFilter {"Entity ID"};

    entityFilter.search({}, ImGuiInputTextFlags_AutoSelectAll);

    for ( const auto&& [eReference, cTag, cTextureTint]
            : registry.view <Tag, TextureTint> ().each() )
    {
      const auto entityId = cTag.id;

      if ( entityId == null_id )
        continue;

      if ( entityFilter.query(entityId.str()) == false )
        continue;

      if ( ImGui::Selectable(entityId.str().c_str(), false) )
      {
        targetTint =
        {
          cTextureTint.tint.r / 255.0f,
          cTextureTint.tint.g / 255.0f,
          cTextureTint.tint.b / 255.0f,
          cTextureTint.tint.a / 255.0f,
        };

        break;
      }
    }
  };

  static std::optional <glm::vec4> tintBuffer {};

  ImGui::Text("Initial");

  ImGui::SameLine();
  if ( ImGui::Button("From TextureTint##initialFromTextureTint") )
    ImGui::OpenPopup("##initialFromTextureTintPopup");

  if ( ImGui::Button("Copy##initialCopy") )
    tintBuffer = mTint.first;

  ImGui::BeginDisabled(tintBuffer.has_value() == false);

  ImGui::SameLine();
  if ( ImGui::Button("Paste##initialPaste") )
    mTint.first = tintBuffer.value();

  ImGui::EndDisabled();

  ImGui::ColorPicker4("##tintInitial", glm::value_ptr(mTint.first));

  ImGui::Text("Target");

  ImGui::SameLine();
  if ( ImGui::Button("From TextureTint##targetFromTextureTint") )
    ImGui::OpenPopup("##targetFromTextureTintPopup");

  if ( ImGui::Button("Copy##targetCopy") )
    tintBuffer = mTint.second;

  ImGui::BeginDisabled(tintBuffer.has_value() == false);

  ImGui::SameLine();
  if ( ImGui::Button("Paste##targetPaste") )
    mTint.second = tintBuffer.value();

  ImGui::EndDisabled();

  ImGui::ColorPicker4("##tintTarget", glm::value_ptr(mTint.second));

  if ( ImGui::BeginPopup("##initialFromTextureTintPopup") )
  {
    setFromTextureTint(registry, mTint.first);
    ImGui::EndPopup(); // initialFromTextureTintPopup
  }

  if ( ImGui::BeginPopup("##targetFromTextureTintPopup") )
  {
    setFromTextureTint(registry, mTint.second);
    ImGui::EndPopup(); // targetFromTextureTintPopup
  }
}

} // namespace cqde::types
