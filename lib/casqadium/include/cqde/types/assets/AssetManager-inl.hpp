#pragma once

#include <cqde/types/assets/AssetManager.hpp>

#include <cqde/common.hpp>
#include <cqde/file_helpers.hpp>

#include <cqde/util/logger.hpp>

#include <json/value.h>
#include <json/reader.h>

#include <thirdparty/ctpl/ctpl_stl.h>


namespace cqde::types
{

template <typename Asset>
std::string
AssetManager <Asset>::StatusAsString(
  const AssetStatus status )
{
  switch (status)
  {
    case AssetStatus::Undefined:
      return "Undefined";

    case AssetStatus::Unloaded:
      return "Unloaded";

    case AssetStatus::Loading:
      return "Loading";

    case AssetStatus::Loaded:
      return "Loaded";

    case AssetStatus::Error:
      return "Error";

    default:
      CQDE_ASSERT_DEBUG(false, return "Undefined");
  }
}

template <typename Asset>
AssetManager <Asset>::AssetManager(
  ctpl::thread_pool& tp )
  : mThreadPool(tp)
{}

template <typename Asset>
AssetManager <Asset>::~AssetManager()
{
  for ( auto& [id, entry] : mAssets )
    unload(id);
}

template <typename Asset>
void
AssetManager <Asset>::parseAssetDb(
  const Json::Value& assetDb,
  const path& dbPath )
{
  std::lock_guard guard(mAssetsMutex);

  for ( const auto& id : assetDb.getMemberNames() )
  {
    try
    {
      parseJsonEntryImpl(assetDb[id], id);

      mAssets[id].path = (dbPath.parent_path() / assetDb[id]["path"].asString()).u8string();
      mAssets[id].handle = {};
      mAssets[id].status = AssetStatus::Unloaded;
    }
    catch ( const std::exception& e )
    {
      LOG_ERROR("Failed to validate JSON entry for asset '{}' ('{}'): {}",
                id, dbPath.string(), e.what());

      mAssets[id].path = path{};
      mAssets[id].handle = {};
      mAssets[id].status = AssetStatus::Error;

      continue;
    }
  }
}

template <typename Asset>
void
AssetManager <Asset>::parseAssetDbFile(
  const path& dbPath )
{
  using fmt::format;

  Json::Value assetDb {};

  LOG_TRACE("Parsing asset DB '{}'", dbPath.string());

  try
  {
    assetDb = fileParse(dbPath);

    if ( assetDb.isObject() == false )
      throw std::runtime_error(
        format("JSON root in '{}' must be an object",
                dbPath.string()));
  }
  catch ( const std::exception& e )
  {
    throw std::runtime_error(
      format("Failed to parse asset DB ({})",
              e.what()));
  }

  parseAssetDb(assetDb, dbPath);
}

template <typename Asset>
void
AssetManager <Asset>::load(
  const std::set <AssetId>& ids )
{
  mThreadPool.push(
  [this, ids] ( const int32_t threadId )
  {
    for ( const auto& id : ids )
    {
      CQDE_ASSERT_DEBUG(id.str().empty() == false, continue);

      mAssetsMutex.lock();

      const AssetStatus assetStatus = status(id);

      if ( assetStatus == AssetStatus::Undefined )
      {
        mAssetsMutex.unlock();
        LOG_ERROR("Can't load unknown asset '{}'", id.str());
        continue;
      }

      else if ( assetStatus >= AssetStatus::Loading )
      {
        mAssetsMutex.unlock();
        continue;
      }

      const AssetPath assetPath = mAssets.at(id).path;

      CQDE_ASSERT_DEBUG(assetPath.string().empty() == false, continue);
      CQDE_ASSERT_DEBUG(assetPath.string() != "***memory***", continue);

      mAssets.at(id).status = AssetStatus::Loading;

      LOG_DEBUG("Loading asset '{}' ('{}')",
                id.str(), assetPath.string());

      mAssetsMutex.unlock();

      AssetHandle resource = loadImpl(id, assetPath);

      std::lock_guard guard(mAssetsMutex);

      if ( status(id) <= AssetStatus::Unloaded )
      {
        unloadImpl(resource);
        continue;
      }

      auto& assetEntry = mAssets.at(id);

      if ( resource != nullptr )
      {
        assetEntry.status = AssetStatus::Loaded;
        assetEntry.handle = std::move(resource);

        continue;
      }

      assetEntry.status = AssetStatus::Error;
      assetEntry.handle = nullptr;
    }
  });
}

template <typename Asset>
void
AssetManager <Asset>::unload(
  const AssetId& id )
{
  std::lock_guard guard(mAssetsMutex);

  CQDE_ASSERT_DEBUG(mAssets.count(id) > 0, return);

  const auto assetPath = mAssets.at(id).path;

  LOG_DEBUG("Unloading asset '{}' ('{}')",
            id.str(), assetPath.string());

  unloadImpl(mAssets.at(id).handle);
  mAssets.at(id).status = AssetStatus::Unloaded;
}

template <typename Asset>
void
AssetManager <Asset>::insert(
  const AssetId& id,
  const AssetHandle res )
{
  std::lock_guard guard(mAssetsMutex);

  mAssets[id].path = "***memory***";
  mAssets[id].handle = res;
  mAssets[id].status = AssetStatus::Loaded;
}

template <typename Asset>
void
AssetManager <Asset>::remove(
  const AssetId& id )
{
  std::lock_guard guard(mAssetsMutex);

  if ( mAssets.count(id) == 0 )
    return;

  unload(id);
  mAssets.erase(id);
}

template <typename Asset>
void
AssetManager <Asset>::clear(
  const bool keepMemoryResidents )
{
  std::lock_guard guard(mAssetsMutex);

  for ( const auto& [id, entry] : mAssets )
    if ( keepMemoryResidents == true &&
         entry.path != "***memory***" )
      unload(id);

  std::vector <std::pair <AssetId, AssetEntry>> assetsInMemory {};

  if ( keepMemoryResidents == true )
    for ( const auto& [id, entry] : mAssets )
      if ( entry.path == "***memory***" )
        assetsInMemory.push_back({id, entry});

  mAssets.clear();
  mAssetsProperties.clear();

  if ( keepMemoryResidents == false )
    return;

  for ( const auto& [id, entry] : assetsInMemory )
    mAssets.insert({id, entry});
}

template <typename Asset>
AssetStatus
AssetManager <Asset>::status(
  const AssetId& id ) const
{
  std::lock_guard guard(mAssetsMutex);

  if ( mAssets.count(id) == 0 )
    return AssetStatus::Undefined;

  return mAssets.at(id).status;
}

template <typename Asset>
std::vector <typename AssetManager <Asset>::AssetId>
AssetManager <Asset>::assetIdList() const
{
  std::lock_guard guard(mAssetsMutex);

  std::vector <AssetId> list {};
  list.reserve(mAssets.size());

  for ( const auto& [id, entry] : mAssets )
    list.push_back(id);

  return list;
}

template <typename Asset>
std::shared_ptr <Asset>
AssetManager <Asset>::try_get(
  const AssetId& id ) const
{
  std::lock_guard guard(mAssetsMutex);

  switch (status(id))
  {
    case AssetStatus::Loading:
      return {};

    case AssetStatus::Loaded:
      return mAssets.at(id).handle;

    default:
      return mAssets.at(null_id).handle;
  }
}

template <typename Asset>
std::shared_ptr <Asset>
AssetManager <Asset>::get(
  const AssetId& id )
{
  std::lock_guard guard(mAssetsMutex);

  if ( status(id) == AssetStatus::Unloaded )
    load({id});

  return try_get(id);
}

} // namespace cqde::types
