#pragma once

#include <cqde/types/ui/PackageFilter.hpp>
#include <cqde/types/ui/StringFilter.hpp>
#include <cqde/types/ui/PackageFilter.hpp>


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
};

} // namespace cqde::ui
