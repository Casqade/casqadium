#include <cqde/components/InteractionListenerColor.hpp>

#include <entt/entity/registry.hpp>

#include <imgui.h>


namespace cqde::compos
{

void
InteractionListenerColor::ui_edit_props(
  const entt::entity,
  const entt::registry& registry )
{
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
}

} // namespace cqde::compos
