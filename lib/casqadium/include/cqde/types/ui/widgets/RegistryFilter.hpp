#pragma once

#include <cqde/types/ui/widgets/PackageFilter.hpp>
#include <cqde/types/ui/widgets/StringFilter.hpp>
#include <cqde/types/ui/widgets/PackageFilter.hpp>


namespace cqde::ui
{

class RegistryFilter
{
  PackageFilter mFilterPackage {};

  StringFilter mFilterEntity    {"Entity ID"};
  StringFilter mFilterComponent {"Component name"};

public:
  RegistryFilter() = default;

  void show( const entt::registry& );

  bool query( const EntityId& );
  bool query( const ComponentType );

  PackageId package() const;

  bool componentExclusive() const;
};

} // namespace cqde::ui
