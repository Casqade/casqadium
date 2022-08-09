#include <cqde/components/GeometryBuffer.hpp>

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
  using types::GeometryAssetManager;

  if ( ImGui::CollapsingHeader("Buffer ID", ImGuiTreeNodeFlags_DefaultOpen) == false )
    return;

  const auto assets = registry.ctx().at <GeometryAssetManager> ().assetIdList();

  if ( ImGui::BeginCombo("##bufferId", buffer.str().c_str(),
                         ImGuiComboFlags_HeightLargest) )
  {
    for ( const auto& asset : assets )
    {
      const bool selected = (buffer == asset);

      if ( ImGui::Selectable(asset.str().c_str(), selected) )
        buffer = asset;
    }
    ImGui::EndCombo();
  }
}

} // namespace cqde::compos
