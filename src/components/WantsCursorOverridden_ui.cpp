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
  using ui::IdSelector;
  using types::MouseCursorAssetManager;

  if ( ImGui::CollapsingHeader("Mouse cursor ID", ImGuiTreeNodeFlags_DefaultOpen) == false )
    return;

  static IdSelector cursorFilter {"Mouse cursor ID", "cursorId"};

  const auto cursors = registry.ctx().get <MouseCursorAssetManager> ().assetIdList();

  cursorFilter.select(
    registry, cursor, cursors );
}

} // namespace cqde::compos
