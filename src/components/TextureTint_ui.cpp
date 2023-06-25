#include <cqde/components/TextureTint.hpp>

#include <entt/entity/registry.hpp>

#include <imgui.h>


namespace cqde::compos
{

void
TextureTint::ui_edit_props(
  const entt::entity,
  const entt::registry& registry )
{
  if ( ImGui::CollapsingHeader("Tint", ImGuiTreeNodeFlags_DefaultOpen) == false )
    return;

  float colorBuffer[4]
  {
    tint.r / 255.0f,
    tint.g / 255.0f,
    tint.b / 255.0f,
    tint.a / 255.0f,
  };

  if ( ImGui::ColorPicker4("##tint", colorBuffer) )
    tint = olc::PixelF( colorBuffer[0], colorBuffer[1],
                        colorBuffer[2], colorBuffer[3]);
}

} // namespace cqde::compos
