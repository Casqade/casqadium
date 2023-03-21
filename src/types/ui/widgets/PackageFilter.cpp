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

  auto packages = registry.ctx().get <PackageManager> ().packages();
  packages.insert(packages.begin(), "");

  ImGui::PushItemWidth(-1.0f);
  if ( ImGui::BeginCombo("##Packages", mPackageSelected.str().c_str()) )
  {
    for ( const auto& package : packages )
    {
      ImGui::PushID(package.hash());

      selected = (package == mPackageSelected);

      if ( ImGui::Selectable(package.str().c_str(), &selected) )
        mPackageSelected = package;

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
