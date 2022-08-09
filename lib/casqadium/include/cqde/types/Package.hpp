#pragma once

#include <cqde/alias.hpp>

#include <entt/entity/registry.hpp>

#include <json/value.h>

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

  PackageId mId {};

public:
  Package( const PackageId& );
  Package( Package&& ) = default;
  Package( const Package& ) = delete;

  enum class ContentType
  {
    Manifest,
    Entities,
    Input,
    Fonts,
    Geometry,
    Textures,
    Text,
  };

  void parseManifest( const path& manifestPath );
  void load( entt::registry& );

  bool save( const ContentType,
             const Json::Value& ) const;

  PackageId   id() const;
  std::string title() const;
  std::string description() const;
  std::string version() const;

  std::set <PackageId> dependencies() const;

  path contentPath( const ContentType ) const;

  static void Validate( const Json::Value& packageManifest );
};

} // namespace cqde::types
