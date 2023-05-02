#include <cqde/components/WantsCursorOverridden.hpp>

#include <cqde/types/ui/widgets/StringFilter.hpp>
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
  using ui::StringFilter;
  using types::MouseCursorAssetManager;

  if ( ImGui::CollapsingHeader("Mouse cursor ID", ImGuiTreeNodeFlags_DefaultOpen) == false )
    return;

  static StringFilter cursorFilter {"Mouse cursor ID"};

  const auto assets = registry.ctx().get <MouseCursorAssetManager> ().assetIdList();

  if ( ImGui::BeginCombo("##cursorId", cursor.str().c_str()) )
  {
    if ( ImGui::IsWindowAppearing() )
      ImGui::SetKeyboardFocusHere(2);

    cursorFilter.search({}, ImGuiInputTextFlags_AutoSelectAll);

    for ( const auto& asset : assets )
    {
      if ( cursorFilter.query(asset.str()) == false )
        continue;

      const bool selected = (cursor == asset);

      if ( ImGui::Selectable(asset.str().c_str(), selected) )
        cursor = asset;
    }
    ImGui::EndCombo();
  }
}

} // namespace cqde::compos
