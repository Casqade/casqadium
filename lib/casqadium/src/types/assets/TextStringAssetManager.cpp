#include <cqde/types/assets/AssetManager.hpp>
#include <cqde/types/assets/AssetManager-inl.hpp>

#include <cqde/util/logger.hpp>

#include <json/value.h>
#include <json/reader.h>


namespace cqde::types
{

template <>
void
AssetManager <std::string>::parseAssetDb(
  const Json::Value& stringDb,
  const std::filesystem::path& dbRootDir )
{
  std::lock_guard guard(mAssetsMutex);

  for ( const auto& id : stringDb.getMemberNames() )
  {
    LOG_ASSERT_DEBUG(id.empty() != true, continue);

    try
    {
      if ( stringDb[id].isString() == true )
      {
        mAssets[id].status = AssetStatus::Unloaded;
        continue;
      }

      if ( stringDb[id].isArray() == true )
      {
        for ( const auto& line : stringDb[id] )
        {
          if ( line.isString() == true )
            continue;

          std::string badJsonEntry = line.toStyledString();
          if ( badJsonEntry.size() > 0 )
            badJsonEntry.pop_back(); // get rid of trailing \n

          throw std::runtime_error(cqde::format("array element '{}' is not a valid JSON string", badJsonEntry));
        }

        mAssets[id].status = AssetStatus::Unloaded;
        continue;
      }

      throw std::runtime_error("JSON value must be a string or an array of strings");
    }
    catch ( const std::exception& e )
    {
      LOG_ERROR("Failed to parse JSON entry for text string '{}': {}",
                id, e.what());

      mAssets[id].handle = {};
      mAssets[id].status = AssetStatus::Error;

      continue;
    }
  }
}

template <>
void
AssetManager <std::string>::parseAssetDb(
  const std::filesystem::path& path )
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


  LOG_DEBUG("Parsing text string DB '{}'", path.string());

  Json::Value stringDb {};

  try
  {
    auto stream = fileOpen(path, std::ios::in);
    stringDb = jsonParse(stream);
  }
  catch ( const std::exception& e )
  {
    throw std::runtime_error(cqde::format("Failed to parse text string DB ({})", e.what()));
  }

  for ( const auto& id : stringDb.getMemberNames() )
  {
    mAssets[id].path = path.string();
    mAssets[id].handle = {};
    mAssets[id].status = AssetStatus::Unloaded;
  }

  LOG_DEBUG("Parsed text string DB '{}'", path.string());
}

template <>
void
AssetManager <std::string>::load(
  const std::set <AssetId>& ids )
{
// TODO: centralize somewhere
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
//

  mThreadPool.push(
  [this, ids, jsonReader] ( const int32_t threadId )
  {
    std::multimap <AssetPath, AssetId> stringPaths {};

    mAssetsMutex.lock();

    for ( const auto& id : ids )
    {
      LOG_ASSERT_DEBUG(id.str().empty() == false, continue);

      const AssetStatus assetStatus = status(id);

      if ( assetStatus == AssetStatus::Undefined )
      {
        LOG_ERROR("Can't load unknown asset '{}'", id.str());
        continue;
      }

      else if ( assetStatus >= AssetStatus::Loading )
        continue;

      const AssetPath assetPath = mAssets.at(id).path;

      LOG_ASSERT_DEBUG(assetPath.str().empty() == false, continue);
      LOG_ASSERT_DEBUG(assetPath.str() != "***memory***", continue);

      mAssets.at(id).status = AssetStatus::Loading;

      stringPaths.insert({assetPath, id});
    }

    mAssetsMutex.unlock();

    Json::Value stringDb {};

    AssetPath pathPrev {};
    for ( const auto& [path, id] : stringPaths )
    {
      LOG_INFO("Loading text string '{}' from '{}'",
               id.str(), path.str());

      if ( path != pathPrev )
      {
        pathPrev = path;
        stringDb.clear();

        LOG_DEBUG("Parsing text string DB '{}'", path.str());

        try
        {
          auto string = fileOpen(path.str(), std::ios::in);
          stringDb = jsonParse(string);
        }
        catch ( const std::exception& e )
        {
          LOG_ERROR("Failed to load text string '{}' ({})",
                    id.str(), e.what());
          continue;
        }

        LOG_DEBUG("Parsed text string DB '{}'", path.str());
      }

      std::shared_ptr <std::string> handle {};

      const Json::Value stringEntry = stringDb[id.str()];

      try
      {
        if ( stringEntry.isString() == true )
          handle = std::make_shared <std::string> (stringEntry.asString());

        else if ( stringEntry.isArray() == true )
        {
          handle = std::make_shared <std::string> ();

          for ( const auto& line : stringEntry )
          {
            if ( line.isString() == true )
              handle->append(line.asString() + "\n");
            else
            {
              std::string badJsonEntry = line.toStyledString();
              if ( badJsonEntry.size() > 0 )
                badJsonEntry.pop_back(); // get rid of trailing \n

              handle.reset();
              throw std::runtime_error(cqde::format("JSON entry '{}' is not a valid JSON string", badJsonEntry));
            }
          }
        }
      }
      catch ( const std::exception& e )
      {
        LOG_ERROR("Failed to load text string '{}': {}",
                  id.str(), e.what());
      }

      std::lock_guard guard(mAssetsMutex);

      if ( status(id) <= AssetStatus::Unloaded )
        continue;

      auto& assetEntry = mAssets.at(id);

      if ( handle != nullptr )
      {
        assetEntry.status = AssetStatus::Loaded;
        assetEntry.handle = std::move(handle);

        LOG_DEBUG("Loaded text string '{}'", id.str());
        continue;
      }

      assetEntry.status = AssetStatus::Error;
      assetEntry.handle = nullptr;
    }
  });
}

template <>
void
AssetManager <std::string>::unloadImpl(
  AssetHandle& handle ) const
{
  LOG_TRACE("Freeing text string");
  handle.reset();
}

template <>
std::shared_ptr <std::string>
AssetManager <std::string>::try_get(
  const AssetId& id ) const
{
  std::lock_guard guard(mAssetsMutex);

  switch (status(id))
  {
    case AssetStatus::Undefined:
      return std::make_shared <std::string> ("STR_NOT_FOUND_" + id.str());

    case AssetStatus::Unloaded:
      return std::make_shared <std::string> ("STR_NOT_LOADED_" + id.str());

    case AssetStatus::Loading:
      return std::make_shared <std::string> ("...");

    case AssetStatus::Error:
      return std::make_shared <std::string> ("STR_ERROR_" + id.str());

    default:
      return mAssets.at(id).handle;
  }
}

template class
AssetManager <std::string>;

} // namespace cqde::types
