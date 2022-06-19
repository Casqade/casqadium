#pragma once

#include <cqde/alias.hpp>

#include <json/value.h>

#include <thirdparty/ctpl/ctpl_stl.h>


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
protected:
  using AssetId = cqde::identifier;
  using AssetPath = cqde::identifier;
  using AssetHandle = std::shared_ptr <Asset>;

  std::map <AssetPath, Json::Value> mAssetsProperties {};

  struct AssetEntry
  {
    AssetPath   path {};
    AssetHandle handle {};
    AssetStatus status {};
  };

public:
  AssetManager() = default;
  ~AssetManager();

  void parseJson( const Json::Value& );

  void load( const std::set <AssetId>& );
  void unload( const AssetId& );

  void insert( const AssetId&, const AssetHandle );

  void parseJsonEntryImpl( const Json::Value& );
  AssetHandle loadImpl( const AssetPath& ) const;
  void unloadImpl( AssetHandle& ) const;

  AssetStatus status( const AssetId& ) const;

  AssetHandle try_get( const AssetId& ) const;
  AssetHandle get( const AssetId& );

  size_t activeThreadCount();

private:
  std::map <AssetId, AssetEntry> mAssets {};

  ctpl::thread_pool mJobs {4};
  mutable std::recursive_mutex mAssetsMutex {};
};

} // namespace cqde::types
