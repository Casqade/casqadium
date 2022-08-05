#include <cqde/types/ui/widgets/RegistryFilter.hpp>

#include <cqde/ecs_helpers.hpp>

#include <imgui.h>


namespace cqde::ui
{

void
RegistryFilter::show(
  const entt::registry& registry )
{
  ImGui::AlignTextToFramePadding();
  ImGui::Text("Package:");

  ImGui::SameLine();
  mFilterPackage.select(registry);

  mFilterComponent.search();
  mFilterEntity.search();
}

bool
RegistryFilter::query(
  const EntityId& entityId )
{
  return mFilterEntity.query(entityId.str());
}

bool
RegistryFilter::query(
  const ComponentType component )
{
  return mFilterComponent.query(component_name(component));
}

PackageId
RegistryFilter::package() const
{
  return mFilterPackage.package();
}

} // namespace cqde::ui
