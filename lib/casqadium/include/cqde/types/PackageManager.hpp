#pragma once

#include <cqde/types/Package.hpp>

#include <unordered_map>


namespace cqde::types
{

class PackageManager
{
  using path = std::filesystem::path;

  path mPackagesRoot {};

  std::unordered_map <PackageId, Package,
                      identifier_hash> mPackages {};

  PackageId mEntryPoint {};

  void parseRoot();

public:
  PackageManager() = default;
  PackageManager( PackageManager&& ) = default;
  PackageManager( const PackageManager& ) = delete;

  void load( const path& packagesRoot, entt::registry& );

  path rootPath() const;
};

} // namespace cqde::types
