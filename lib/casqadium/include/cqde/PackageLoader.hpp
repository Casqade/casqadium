#pragma once

#include <cqde/alias.hpp>

#include <entt/entt.hpp>

#include <filesystem>


namespace cqde
{

namespace compos
{

struct Package
{
  std::filesystem::path path;
  entt::registry registry;
  PackageId title;


  void open( const std::filesystem::path& );
  void save( const std::filesystem::path& );

  void load( const entt::registry& globalRegistry );
};

struct PackageManager
{
  std::map <PackageId, Package> packages;
};

} // namespace compos

class PackageLoader
{
  std::filesystem::path mPackagesRootPath;

public:
  PackageLoader( const std::filesystem::path& );

  bool Load();
};

} // namespace cqde
