#pragma once

#include <cqde/types/assets/AssetManager.hpp>

#include <cqde/util/logger.hpp>

#include <json/value.h>
#include <json/reader.h>


namespace cqde::types
{

template <typename Asset>
AssetManager <Asset>::~AssetManager()
{
  for ( auto& [id, entry] : mAssets )
    unload(id);
}

template <typename Asset>
void
AssetManager <Asset>::parseJson(
  const Json::Value& assetList,
  const std::string& packageDir )
{
  LOG_ASSERT_DEBUG(packageDir.empty() != true, return);

  std::lock_guard guard(mAssetsMutex);

  for ( const auto& id : assetList.getMemberNames() )
  {
    LOG_ASSERT_DEBUG(id.empty() != true, continue);

    try
    {
      if ( assetList[id].empty() == true )
        throw std::runtime_error("JSON value is empty");

      if ( assetList[id].isObject() == false )
      {
        std::string entryString = assetList[id].toStyledString();
        if ( entryString.size() > 0 )
          entryString.pop_back(); // get rid of trailing \n

        throw std::runtime_error(cqde::format("'{}' is not a valid JSON object",
                                              entryString));
      }

      const Json::Value assetPath = assetList[id]["path"];

      if (    assetPath.isNull() == true
           || assetPath.isString() == false
           || assetPath.asString().empty() == true )
        throw std::runtime_error("file path is undefined");

      parseJsonEntryImpl(assetList[id], id);
    }
    catch ( const std::exception& e )
    {
      LOG_ERROR("Failed to parse JSON entry for asset '{}': {}",
                id, e.what());

      mAssets[id].path = {};
      mAssets[id].handle = {};
      mAssets[id].status = AssetStatus::Error;

      continue;
    }

    mAssets[id].path = (std::filesystem::path(packageDir) / assetList[id]["path"].asString()).u8string();
    mAssets[id].handle = {};
    mAssets[id].status = AssetStatus::Unloaded;
  }
}

template <typename Asset>
void
AssetManager <Asset>::parseFile(
  const std::string& path )
{
  Json::CharReaderBuilder jsonReader {};

  jsonReader["collectComments"] = true;
  jsonReader["allowComments"] = true;
  jsonReader["allowTrailingCommas"] = true;
  jsonReader["strictRoot"] = false;
  jsonReader["allowDroppedNullPlaceholders"] = false;
  jsonReader["allowNumericKeys"] = false;
  jsonReader["allowSingleQuotes"] = false;
  jsonReader["stackLimit"] = 1000;
  jsonReader["failIfExtra"] = false;
  jsonReader["rejectDupKeys"] = true;
  jsonReader["allowSpecialFloats"] = true;
  jsonReader["skipBom"] = true;


  LOG_DEBUG("Opening asset DB '{}'", path);

  Json::Value assetDb {};

  std::ifstream assetDbFile( path, std::ios::in );

  try
  {
    if ( assetDbFile.is_open() == false )
      throw std::runtime_error("Can't open DB file");

    LOG_DEBUG("Parsing asset DB '{}'", path);

    Json::String parseErrors {};

    if ( Json::parseFromStream( jsonReader, assetDbFile,
                                &assetDb, &parseErrors ) == false )
      throw std::runtime_error(parseErrors);
  }
  catch ( const std::exception& e )
  {
    if ( assetDbFile.is_open() == true )
      assetDbFile.close();

    throw std::runtime_error(cqde::format("Failed to parse asset DB '{}': {}",
                                          path, e.what()));
  }
  assetDbFile.close();

  parseJson(assetDb, std::filesystem::path(path).remove_filename());

  LOG_DEBUG("Parsed asset DB '{}'", path);
}

template <typename Asset>
void
AssetManager <Asset>::load(
  const std::set <AssetId>& ids )
{
  mJobs.push(
  [this, ids] ( const int32_t threadId )
  {
    for ( const auto& id : ids )
    {
      LOG_ASSERT_DEBUG(id.str().empty() == false, continue);

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

      LOG_ASSERT_DEBUG(assetPath.str().empty() == false, continue);
      LOG_ASSERT_DEBUG(assetPath.str() != "***memory***", continue);

      mAssets.at(id).status = AssetStatus::Loading;

      LOG_DEBUG("Loading asset '{}' ('{}')",
                id.str(), assetPath.str());

      mAssetsMutex.unlock();

      try
      {
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

          LOG_DEBUG("Loaded asset '{}' ('{}')",
                    id.str(), assetPath.str());
          continue;
        }

        assetEntry.status = AssetStatus::Error;
        assetEntry.handle = nullptr;
      }
      catch ( const std::exception& e )
      {
        LOG_ERROR("Failed to load asset '{}' ('{}'): {}",
                  id.str(), assetPath.str(), std::string(e.what()));
      }
    }
  });
}

template <typename Asset>
void
AssetManager <Asset>::unload(
  const AssetId& id )
{
  std::lock_guard guard(mAssetsMutex);

  const std::string assetPath = mAssets.at(id).path.str();

  LOG_DEBUG("Unloading asset '{}' ('{}')",
            id.str(), assetPath);

  unloadImpl(mAssets.at(id).handle);
  mAssets.at(id).status = AssetStatus::Unloaded;

  LOG_DEBUG("Unloaded asset '{}' ('{}')",
            id.str(), assetPath);
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
AssetStatus
AssetManager <Asset>::status(
  const AssetId& id ) const
{
  LOG_ASSERT_DEBUG(id.str().empty() == false,
                   return AssetStatus::Undefined);

  std::lock_guard guard(mAssetsMutex);

  if ( mAssets.count(id) == 0 )
    return AssetStatus::Undefined;

  return mAssets.at(id).status;
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
      return mAssets.at(cqde::null_id).handle;
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

template <typename Asset>
size_t
AssetManager <Asset>::activeThreadCount()
{
  return mJobs.size() - mJobs.n_idle();
}

} // namespace cqde::types
