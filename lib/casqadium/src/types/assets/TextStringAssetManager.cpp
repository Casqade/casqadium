#include <cqde/types/assets/AssetManager-inl.hpp>

#include <cqde/json_helpers.hpp>

#include <cqde/util/logger.hpp>

#include <spdlog/fmt/bundled/format.h>

#include <json/value.h>
#include <json/reader.h>


namespace cqde::types
{

template <>
Json::Value
AssetManager <std::string>::AssetJsonDbEntryReference()
{
  using ValueType = Json::ValueType;
  using namespace std::string_literals;

  Json::Value reference = ValueType::arrayValue;
  reference.setComment("// text string value must be either a string or a JSON array"s,
                       Json::CommentPlacement::commentBefore);

  reference.append(ValueType::stringValue);
  reference.begin()->setComment("// text string array element must be a JSON string"s,
                                Json::CommentPlacement::commentBefore);

  return reference;
}

template <>
void
AssetManager <std::string>::parseAssetDb(
  const Json::Value& stringDb,
  const path& dbPath )
{
  std::lock_guard guard(mAssetsMutex);

  for ( const auto& id : stringDb.getMemberNames() )
  {
    try
    {
      if ( stringDb[id].isString() == false )
        jsonValidateArray(stringDb[id], AssetJsonDbEntryReference());

      mAssets[id].path = dbPath;
      mAssets[id].handle = {};
      mAssets[id].status = AssetStatus::Unloaded;

      continue;
    }
    catch ( const std::exception& e )
    {
      LOG_ERROR("Failed to validate JSON entry for text string '{}' ('{}'): {}",
                id, dbPath.string(), e.what());

      mAssets[id].path = path{};
      mAssets[id].handle = {};
      mAssets[id].status = AssetStatus::Error;

      continue;
    }
  }
}

template <>
void
AssetManager <std::string>::load(
  const std::set <AssetId>& ids )
{
  using fmt::format;

  mThreadPool.push(
  [this, ids] ( const int32_t threadId )
  {
    std::multimap <AssetPath, AssetId> stringPaths {};

    mAssetsMutex.lock();

    for ( const auto& id : ids )
    {
      CQDE_ASSERT_DEBUG(id.str().empty() == false, continue);

      const AssetStatus assetStatus = status(id);

      if ( assetStatus == AssetStatus::Undefined )
      {
        LOG_ERROR("Can't load unknown text string '{}'", id.str());
        continue;
      }

      else if ( assetStatus >= AssetStatus::Loading )
        continue;

      const AssetPath assetPath = mAssets.at(id).path;

      CQDE_ASSERT_DEBUG(assetPath.empty() == false, continue);
      CQDE_ASSERT_DEBUG(assetPath.string() != "***memory***", continue);

      mAssets.at(id).status = AssetStatus::Loading;

      stringPaths.insert({assetPath, id});
    }

    mAssetsMutex.unlock();

    Json::Value stringDb {};

    bool dbHasErrors = false;

    AssetPath pathPrev {};
    for ( const auto& [path, id] : stringPaths )
    {
      LOG_DEBUG("Loading text string '{}' ('{}')",
                id.str(), path.string());

      if ( path == pathPrev && dbHasErrors == true )
      {
        LOG_ERROR("Failed to load text string '{}' (errors in DB '{}')",
                  id.str(), path.string());
        continue;
      }

      if ( path != pathPrev )
      {
        pathPrev = path;
        stringDb.clear();

        LOG_TRACE("Parsing text string DB '{}'", path.string());

        try
        {
          stringDb = fileParse(path);

          if ( stringDb.isObject() == false )
            throw std::runtime_error(
              format( "JSON root in '{}' must be an object",
                      path.string()));
        }
        catch ( const std::exception& e )
        {
          dbHasErrors = true;
          LOG_ERROR("Failed to parse text string DB ({})", e.what());
          LOG_ERROR("Failed to load text string '{}' (invalid DB '{}')",
                    id.str(), path.string());
          continue;
        }
        dbHasErrors = false;
      }

      std::shared_ptr <std::string> handle {};

      const Json::Value stringEntry = stringDb[id.str()];

      try
      {
        if ( stringEntry.isString() == true )
          handle = std::make_shared <std::string> (stringEntry.asString());

        else
        {
          jsonValidateArray(stringEntry, AssetJsonDbEntryReference());

          handle = std::make_shared <std::string> ();

          for ( const auto& line : stringEntry )
            handle->append(line.asString() + "\n");
        }
      }
      catch ( const std::exception& e )
      {
        LOG_ERROR("Failed to load text string '{}' ('{}'): {}",
                  id.str(), path.string(), e.what());
      }

      std::lock_guard guard(mAssetsMutex);

      if ( status(id) <= AssetStatus::Unloaded )
        continue;

      auto& assetEntry = mAssets.at(id);

      if ( handle != nullptr )
      {
        assetEntry.status = AssetStatus::Loaded;
        assetEntry.handle = std::move(handle);

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
