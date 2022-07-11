#pragma once

#include <cqde/alias.hpp>

#include <json/value.h>

#include <filesystem>
#include <mutex>
#include <set>


namespace ctpl
{
class thread_pool;
}

namespace cqde::types
{

enum class AssetStatus
{
  Undefined,
  Unloaded,
  Loading,
  Loaded,
  Error,
};

template <typename Asset>
class AssetManager
{
  using path = std::filesystem::path;

  using AssetId = cqde::identifier;
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

public:
  AssetManager( ctpl::thread_pool& );
  ~AssetManager();

  static Json::Value AssetJsonDbEntryReference();

  void parseAssetDbFile(const path& );
  void parseAssetDb(const Json::Value& db,
                    const path& dbPath );

  void load( const std::set <AssetId>& );
  void unload( const AssetId& );

  void insert( const AssetId&, const AssetHandle );

  void parseJsonEntryImpl( const Json::Value&,
                           const AssetId& );
  AssetHandle loadImpl( const AssetId&,
                        const AssetPath& ) const;
  void unloadImpl( AssetHandle& ) const;

  AssetStatus status( const AssetId& ) const;
  std::vector <AssetId> assetIdList() const;

  AssetHandle try_get( const AssetId& ) const;
  AssetHandle get( const AssetId& );
};

} // namespace cqde::types
