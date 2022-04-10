#ifndef ECS_PACKAGE_LOADER_HPP
#define ECS_PACKAGE_LOADER_HPP

#include <ECS/Alias.hpp>

#include <entt/entt.hpp>

#include <filesystem>


namespace ECS
{

namespace Components
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

} // namespace Components

class PackageLoader
{
  std::filesystem::path mPackagesRootPath;

public:
  PackageLoader( const std::filesystem::path& );

  bool Load();
};

} // namespace ECS

#endif
