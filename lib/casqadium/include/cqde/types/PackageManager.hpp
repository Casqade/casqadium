#pragma once

#include <cqde/types/Package.hpp>

#include <unordered_map>


namespace cqde::types
{

class PackageManager
{
  std::filesystem::path mPackagesRoot {};

  std::unordered_map <PackageId, Package,
                      identifier_hash> mPackages {};

  PackageId mEntryPoint {};

  void parseRoot();

public:
  PackageManager() = default;
  PackageManager( PackageManager&& ) = default;
  PackageManager( const PackageManager& ) = delete;

  void load( const std::filesystem::path& packagesRoot, entt::registry& );

  std::filesystem::path rootPath() const;
};

} // namespace cqde::types
