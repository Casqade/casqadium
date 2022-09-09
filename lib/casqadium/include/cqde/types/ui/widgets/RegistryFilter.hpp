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

  bool query( const entt::registry&, const entt::entity ) const;
  bool query( const EntityId& ) const;
  bool query( const ComponentType ) const;

  PackageId package() const;
};

} // namespace cqde::ui
