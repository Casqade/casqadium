#include <cqde/types/ui/widgets/RegistryFilter.hpp>

#include <cqde/components/Tag.hpp>
#include <cqde/components/EntityMetaInfo.hpp>

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
  const entt::registry& registry,
  const entt::entity entity ) const
{
  using compos::Tag;
  using compos::EntityMetaInfo;

  const auto packageSelected = package();

  if ( packageSelected.str().empty() == false )
  {
    const auto cMetaInfo = registry.try_get <EntityMetaInfo> (entity);

    if ( cMetaInfo == nullptr ||
         packageSelected != cMetaInfo->packageId )
      return false;
  }

  const auto entityId = registry.get <Tag> (entity).id.str();

  if ( query(entityId) == false )
    return false;

  bool componentFound = mFilterComponent.exclusive();

  each_component(entity, registry,
  [this, &componentFound] ( const ComponentType component )
  {
    if ( mFilterComponent.exclusive() == false )
    {
      componentFound = query(component);

      if ( componentFound == true )
        return false;
    }
    else
      componentFound &= query(component);

    return true;
  });

  return componentFound;
}

bool
RegistryFilter::query(
  const EntityId& entityId ) const
{
  return mFilterEntity.query(entityId.str());
}

bool
RegistryFilter::query(
  const ComponentType component ) const
{
  return mFilterComponent.query(component_name(component));
}

PackageId
RegistryFilter::package() const
{
  return mFilterPackage.package();
}

} // namespace cqde::ui
