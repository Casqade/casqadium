#pragma once

#include <cqde/types/Package.hpp>

#include <vector>


namespace cqde::ui
{
class PackageManagerUi;
}

namespace cqde::types
{

class PackageManager
{
  friend class ui::PackageManagerUi;

  using path = std::filesystem::path;

  path mPackagesRoot {};

  std::vector <Package> mPackages {};

  PackageId mEntryPoint {};

  void parseRoot();

public:
  PackageManager() = default;
  PackageManager( PackageManager&& ) = default;
  PackageManager( const PackageManager& ) = delete;

  void load( const path& packagesRoot, entt::registry& );

  static void Validate( const Json::Value& packagesRoot );

  const Package* package( const PackageId& ) const;

  std::vector <PackageId> packages() const;
  path rootPath() const;
};

} // namespace cqde::types
