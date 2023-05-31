#pragma once

#include <cqde/alias.hpp>
#include <cqde/types/graphics/Mesh.hpp>
#include <cqde/types/graphics/MeshStorage.hpp>

#include <cqde/types/assets/AssetStatus.hpp>

#include <entt/fwd.hpp>

#include <json/forwards.h>

#include <map>
#include <mutex>
#include <filesystem>
#include <unordered_set>


namespace cqde::types
{

class MeshAssetManager
{
  using Path = std::filesystem::path;

  using AssetId = identifier;
  using AssetHandle = std::shared_ptr <Mesh>;
  using AssetIdList = std::unordered_set <AssetId, identifier_hash>;

  MeshStorage mMeshes {};

  struct AssetEntry
  {
    Path        path {};
    AssetHandle handle {};
    AssetStatus status {};
  };


  std::map <AssetId, AssetEntry> mAssets {};

  mutable std::recursive_mutex mAssetsMutex {};

  void initPlaceholders();

  AssetHandle loadImpl(
    const AssetId&,
    const Path& );

  void fitMesh( Mesh& segment ) const;

public:
  MeshAssetManager();
  ~MeshAssetManager();

  static Json::Value AssetJsonDbEntryReference();
  static void Validate( const Json::Value& );

  void parseAssetDbFile( const Path& dbPath );
  void parseAssetDb(
    const Json::Value& assetDb,
    const Path& dbPath );

  void load( const AssetIdList& );
  void unload( const AssetIdList& );

  void insert(
    const AssetId&,
    AssetHandle res );

  void remove( const AssetId& );

  void clear( const bool keepMemoryResidents = true );

  AssetStatus status( const AssetId& ) const;

  std::vector <AssetId> assetIdList() const;

  std::shared_ptr <Mesh> try_get(
    const AssetId& ) const;

  std::shared_ptr <Mesh> get(
    const AssetId& );

  uint32_t vaoId() const;

  void ui_show_preview( const AssetId&, entt::registry& );
  void ui_show( Json::Value& entry ) const;
};

} // namespace cqde::types
