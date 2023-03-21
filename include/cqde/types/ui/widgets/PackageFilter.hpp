#pragma once

#include <cqde/alias.hpp>

#include <entt/fwd.hpp>


namespace cqde::ui
{

class PackageFilter
{
  PackageId mPackageSelected {""};

public:
  PackageFilter() = default;

  bool select( const entt::registry& );

  PackageId package() const;
};

} // namespace cqde::UI
