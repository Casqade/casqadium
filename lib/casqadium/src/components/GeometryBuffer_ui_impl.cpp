#include <cqde/components/GeometryBuffer.hpp>

#include <cqde/types/ui/widgets/StringFilter.hpp>
#include <cqde/types/assets/GeometryAssetManager.hpp>

#include <entt/entity/registry.hpp>

#include <imgui.h>


namespace cqde::compos
{

void
GeometryBuffer::ui_edit_props(
  const entt::entity,
  const entt::registry& registry )
{
  using ui::StringFilter;
  using types::GeometryAssetManager;

  if ( ImGui::CollapsingHeader("Buffer ID", ImGuiTreeNodeFlags_DefaultOpen) == false )
    return;

  static StringFilter geometryFilter {"Geometry ID"};

  const auto assets = registry.ctx().at <GeometryAssetManager> ().assetIdList();

  if ( ImGui::BeginCombo("##geometryId", buffer.str().c_str()) )
  {
    if ( ImGui::IsWindowAppearing() )
      ImGui::SetKeyboardFocusHere(2);

    geometryFilter.search({}, ImGuiInputTextFlags_AutoSelectAll);

    for ( const auto& asset : assets )
    {
      if ( geometryFilter.query(asset.str()) == false )
        continue;

      const bool selected = (buffer == asset);

      if ( ImGui::Selectable(asset.str().c_str(), selected) )
        buffer = asset;
    }
    ImGui::EndCombo();
  }
}

} // namespace cqde::compos
