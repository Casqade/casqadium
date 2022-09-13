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

  std::set <PackageId> mDependencies {};

  std::string mTitle {};
  std::string mDescription {};
  std::string mVersion {};

  PackageId mId {};

public:
  Package( const PackageId& );

  enum class ContentType
  {
    Manifest,
    Entities,
    Prefabs,
    Input,
    Audio,
    Fonts,
    Geometry,
    Textures,
    Text,
  };

  void parseManifest( const path& manifestPath );

  void load( entt::registry& );

  bool save( const ContentType,
             const Json::Value& ) const;

  void ui_show( Json::Value& );

  PackageId   id() const;
  std::string title() const;
  std::string description() const;
  std::string version() const;

  std::set <PackageId> dependencies() const;

  path contentPath( const ContentType ) const;

  static std::string ContentFileName( const ContentType );

  static void Validate( const Json::Value& packageManifest );
};

} // namespace cqde::types
