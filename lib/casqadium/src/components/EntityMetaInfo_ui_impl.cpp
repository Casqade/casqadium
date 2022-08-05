#include <cqde/components/EntityMetaInfo.hpp>

#include <cqde/types/PackageManager.hpp>

#include <entt/entity/registry.hpp>

#include <imgui.h>


namespace cqde::compos
{

void
EntityMetaInfo::ui_edit_props(
  const entt::entity,
  const entt::registry& registry )
{
  using types::PackageManager;

  if ( ImGui::CollapsingHeader("Package ID", ImGuiTreeNodeFlags_DefaultOpen) == false )
    return;

  const auto& pkgMgr = registry.ctx().at <PackageManager> ();
  const auto packages = pkgMgr.packages();

  if ( ImGui::BeginCombo("##packageId", packageId.str().c_str()) )
  {
    for ( const auto& packageId : packages )
    {
      const bool selected = (packageId == this->packageId );

      if ( ImGui::Selectable(packageId.str().c_str(), selected) )
        this->packageId = packageId;
    }
    ImGui::EndCombo();
  }
}

} // namespace cqde::compos
