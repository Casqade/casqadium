#pragma once

#include <cqde/alias.hpp>

#include <json/value.h>

#include <mutex>


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
  using AssetId = cqde::identifier;
  using AssetPath = cqde::identifier;
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

  void parseFile( const std::string& path );
  void parseJson( const Json::Value&,
                  const std::string& packageDir );

  void load( const std::set <AssetId>& );
  void unload( const AssetId& );

  void insert( const AssetId&, const AssetHandle );

  void parseJsonEntryImpl( const Json::Value&,
                           const AssetId& );
  AssetHandle loadImpl( const AssetId&,
                        const AssetPath& ) const;
  void unloadImpl( AssetHandle& ) const;

  AssetStatus status( const AssetId& ) const;

  AssetHandle try_get( const AssetId& ) const;
  AssetHandle get( const AssetId& );
};

} // namespace cqde::types
