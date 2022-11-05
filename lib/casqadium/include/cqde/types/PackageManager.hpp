#pragma once

#include <cqde/fwd.hpp>
#include <cqde/types/Package.hpp>

#include <vector>


namespace cqde::types
{

class PackageManager
{
  friend class ui::PackageManagerUi;

  using path = std::filesystem::path;

  path mPackagesRootPath {"data"};

  std::vector <Package> mPackages {};

  PackageId mEntryPoint {};

  void parseManifest();

public:
  PackageManager() = default;

  void setRootPath( const path& packagesRoot );

  void load( entt::registry& );
  void unload();

  void create( const PackageId&, const Json::Value& manifest );

  static void Validate( const Json::Value& packagesManifest );

  const Package* package( const PackageId& ) const;

  PackageId entryPoint() const;
  std::vector <PackageId> packages() const;

  path rootPath() const;
  path manifestPath() const;
};

} // namespace cqde::types
