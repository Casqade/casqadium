#include <cqde/types/assets/AssetManager.hpp>
#include <cqde/types/assets/AssetManager-inl.hpp>

#include <cqde/util/logger.hpp>

#include <json/value.h>
#include <json/reader.h>


namespace cqde::types
{

template <>
void
AssetManager <std::string>::parseJson(
  const Json::Value& stringList )
{
  std::lock_guard guard(mAssetsMutex);

  for ( const auto& id : stringList.getMemberNames() )
  {
    LOG_ASSERT_DEBUG(id.empty() != true, continue);

    try
    {
      if ( stringList[id].isString() == true )
      {
        mAssets[id].status = AssetStatus::Unloaded;
        continue;
      }

      if ( stringList[id].isArray() == true )
      {
        for ( const auto& line : stringList[id] )
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
AssetManager <std::string>::parseFile(
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


  LOG_DEBUG("Opening text string DB '{}'", path);

  Json::Value stringDb {};

  std::ifstream stringDbFile( path, std::ios::in );

  try
  {
    if ( stringDbFile.is_open() == false )
      throw std::runtime_error("Can't open DB file");

    LOG_DEBUG("Parsing text string DB '{}'", path);

    Json::String parseErrors {};

    if ( Json::parseFromStream( jsonReader, stringDbFile,
                                &stringDb, &parseErrors ) == false )
      throw std::runtime_error(parseErrors);
  }
  catch ( const std::exception& e )
  {
    if ( stringDbFile.is_open() == true )
      stringDbFile.close();

    throw std::runtime_error(cqde::format("Failed to parse text string DB '{}': {}",
                                          path, e.what()));
  }
  stringDbFile.close();

  for ( const auto& id : stringDb.getMemberNames() )
  {
    mAssets[id].path = path;
    mAssets[id].handle = {};
    mAssets[id].status = AssetStatus::Unloaded;
  }
}

template <>
void
AssetManager <std::string>::load(
  const std::set <AssetId>& ids )
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


  mJobs.push(
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

        LOG_DEBUG("Opening text string DB '{}'", path.str());

        std::ifstream stringDbFile( path.str(), std::ios::in );
        if ( stringDbFile.is_open() == false )
        {
          LOG_ERROR("Failed to load text string '{}': can't open text string DB '{}'",
                    id.str(), path.str());
          continue;
        }

        LOG_DEBUG("Parsing text string DB '{}'", path.str());

        Json::String parseErrors {};

        if ( Json::parseFromStream( jsonReader, stringDbFile,
                                    &stringDb, &parseErrors ) == false )
        {
          stringDbFile.close();
          LOG_ERROR("Failed to load text string '{}', text string DB '{}' contains errors: {}",
                    id.str(), path.str(), parseErrors);
          continue;
        }
        stringDbFile.close();
      }

      std::shared_ptr <std::string> handle {};

      const Json::Value stringEntry = stringDb[id.str()];

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

            LOG_ERROR("Failed to parse JSON entry for text string '{}': '{}' is not a valid JSON string",
                      id.str(), badJsonEntry);

            handle.reset();
            break;
          }
        }
      }

      std::lock_guard guard(mAssetsMutex);

      if ( status(id) <= AssetStatus::Unloaded )
        continue;

      auto& assetEntry = mAssets.at(id);

      if ( handle != nullptr )
      {
        assetEntry.status = AssetStatus::Loaded;
        assetEntry.handle = std::move(handle);

        LOG_DEBUG("Loaded text string '{}' from '{}'",
                  id.str(), path.str());

        continue;
      }

      assetEntry.status = AssetStatus::Error;
      assetEntry.handle = nullptr;

      LOG_ERROR("Failed to load text string '{}' from '{}'",
                id.str(), path.str());
    }
  });
}

template <>
void
AssetManager <std::string>::unloadImpl(
  AssetHandle& handle ) const
{
  LOG_INFO("Destroying text string");
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

    case AssetStatus::Loaded:
      return mAssets.at(id).handle;

    case AssetStatus::Error:
      return std::make_shared <std::string> ("STR_ERROR_" + id.str());
  }
}

template class
AssetManager <std::string>;

} // namespace cqde::types
