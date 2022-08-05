#include <cqde/types/ui/widgets/PackageFilter.hpp>

#include <cqde/types/PackageManager.hpp>

#include <entt/entity/registry.hpp>

#include <imgui.h>


namespace cqde::ui
{

bool
PackageFilter::select(
  const entt::registry& registry )
{
  using types::PackageManager;

  bool selected {};

  auto packages = registry.ctx().at <PackageManager> ().packages();
  packages.insert(packages.begin(), "");

  ImGui::PushItemWidth(-1.0f);
  if ( ImGui::BeginCombo("##Packages", mPackageSelected.str().c_str()) )
  {
    for ( const auto& package : packages )
    {
      selected = (package == mPackageSelected);

      ImGui::PushID(package.hash().value());
      if ( ImGui::Selectable(package.str().c_str(), &selected) )
        mPackageSelected = package;

      if ( selected == true )
        ImGui::SetItemDefaultFocus();

      ImGui::PopID();
    }
    ImGui::EndCombo();
  }

  return selected;
}

PackageId
PackageFilter::package() const
{
  return mPackageSelected;
}

} // namespace cqde::ui
