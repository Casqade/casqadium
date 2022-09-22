#pragma once

#include <cqde/alias.hpp>

#include <entt/fwd.hpp>

#include <json/value.h>

#include <vector>
#include <filesystem>


namespace cqde::types
{

struct Snapshot
{
  std::filesystem::path path {};

  std::filesystem::file_time_type lastModified {};


  Snapshot() = default;
};

class SnapshotManager
{
  using path = std::filesystem::path;

  std::vector <Snapshot> mSnapshots {};

public:
  SnapshotManager() = default;

  void readFromDisk( entt::registry& );

  std::vector <Snapshot> snapshots() const;

  static void Validate( const Json::Value& );
  static void Validate( const path& );

  static Json::Value Create( const entt::registry& );

  static void Write( const entt::registry&,
                     const std::string& filename );
  static void Load( entt::registry&,
                    const path& );
};

} // namespace cqde::types
