#pragma once

#include <cqde/alias.hpp>

#include <cqde/types/assets/AssetStatus.hpp>

#include <entt/fwd.hpp>

#include <json/value.h>

#include <filesystem>
#include <mutex>
#include <unordered_set>


namespace ctpl
{
class thread_pool;
}

namespace cqde::types
{

template <typename Asset>
class AssetManager
{
  using path = std::filesystem::path;

  using AssetId = identifier;
  using AssetPath = std::filesystem::path;
  using AssetHandle = std::shared_ptr <Asset>;

  struct AssetEntry
  {
    AssetPath   path {};
    AssetHandle handle {};
    AssetStatus status {};
  };


  ctpl::thread_pool& mThreadPool;

  std::map <AssetId, AssetEntry> mAssets {};
  std::map <AssetId, Json::Value> mAssetsProperties {};

  mutable std::recursive_mutex mAssetsMutex {};


  void initPlaceholders();

  void parseJsonEntryImpl( const Json::Value&,
                           const AssetId& );
  AssetHandle loadImpl( const AssetId&,
                        const AssetPath& ) const;
  void unloadImpl( AssetHandle& ) const;

public:
  AssetManager( ctpl::thread_pool& );
  ~AssetManager();

  static Json::Value AssetJsonDbEntryReference();
  static void Validate( const Json::Value& );

  void parseAssetDbFile( const path& );
  void parseAssetDb(  const Json::Value& db,
                      const path& dbPath );

  void load( const std::unordered_set <AssetId, identifier_hash>& );
  void unload( const std::unordered_set <AssetId, identifier_hash>& );

  void insert( const AssetId&, const AssetHandle );
  void remove( const AssetId& );
  void clear( const bool keepMemoryResidents = true );

  void ui_show_preview( const AssetId&, entt::registry& );
  void ui_show( Json::Value& ) const;

  AssetStatus status( const AssetId& ) const;
  std::vector <AssetId> assetIdList() const;

  AssetHandle try_get( const AssetId& ) const;
  AssetHandle get( const AssetId& );
};

} // namespace cqde::types
