#include <cqde/components/WantsCursorOverridden.hpp>

#include <cqde/types/ui/widgets/IdSelector.hpp>
#include <cqde/types/assets/MouseCursorAssetManager.hpp>

#include <entt/entity/registry.hpp>

#include <imgui.h>


namespace cqde::compos
{

void
WantsCursorOverridden::ui_edit_props(
  const entt::entity,
  const entt::registry& registry )
{
  using types::MouseCursorAssetManager;

  if ( ImGui::CollapsingHeader("Mouse cursor ID", ImGuiTreeNodeFlags_DefaultOpen) == false )
    return;

  static ui::IdSelector cursorFilter {
    "Mouse cursor ID", "##cursorId" };

  auto& cursors = registry.ctx().get <MouseCursorAssetManager> ();

  cursorFilter.selectCombo(
    cursor, cursors.assetIdList() );
}

} // namespace cqde::compos
