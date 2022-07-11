#pragma once

#include <cqde/alias.hpp>

#include <entt/entity/registry.hpp>

#include <filesystem>
#include <set>


namespace cqde::types
{

class Package
{
  using path = std::filesystem::path;

  path mManifestPath {};

  entt::registry mRegistry {};

  std::set <PackageId> mDependencies {};

  std::string mTitle {};
  std::string mDescription {};
  std::string mVersion {};


public:
  Package() = default;
  Package( Package&& ) = default;
  Package( const Package& ) = delete;

  void parseManifest( const path& manifestPath );
  void load( entt::registry& );

  std::set <PackageId> dependencies() const;

  path manifestPath() const;
};

} // namespace cqde::types
