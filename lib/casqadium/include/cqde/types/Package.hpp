#pragma once

#include <cqde/alias.hpp>

#include <entt/entity/registry.hpp>

#include <filesystem>
#include <set>


namespace cqde::types
{

class Package
{
  std::filesystem::path mManifestPath {};

  entt::registry mRegistry {};

  std::set <PackageId> mDependencies {};

  std::string mTitle {};
  std::string mDescription {};
  std::string mVersion {};


public:
  Package() = default;
  Package( Package&& ) = default;
  Package( const Package& ) = delete;

  void parseManifest( const std::filesystem::path& manifestPath );
  void load( entt::registry& );

  std::set <PackageId> dependencies() const;
};

} // namespace cqde::types
