#include <cqde/types/assets/AssetManager-inl.hpp>

#include <cqde/common.hpp>
#include <cqde/json_helpers.hpp>

#include <cqde/util/logger.hpp>

#include <json/value.h>
#include <json/reader.h>

#include <imgui.h>
#include <imgui_stdlib.h>


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
AssetManager <std::string>::Validate(
  const Json::Value& json )
{
//  using ValueType = Json::ValueType;
//  using namespace std::string_literals;

//  Json::Value reference = ValueType::objectValue;
//  reference.setComment("// text string DB must be a JSON object"s,
//                       Json::CommentPlacement::commentBefore);

//  Json::Value& asset = reference["cqde_json_anykey"];
//  asset = AssetJsonDbEntryReference();

//  jsonValidateObject(json, reference);
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
      CQDE_ASSERT_DEBUG(assetPath.string() != MemoryResidentPath, continue);

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

template <>
void
AssetManager <std::string>::ui_show_preview(
  const AssetId& stringId )
{
  using fmt::format;

  const auto assetStatus = status(stringId);

  if ( ImGui::CollapsingHeader("Status", ImGuiTreeNodeFlags_DefaultOpen) )
    ImGui::TextColored(ImGui::ColorConvertU32ToFloat4(StatusAsColor(assetStatus)),
                       "%s", StatusAsString(assetStatus).c_str());

  if ( assetStatus == AssetStatus::Undefined )
    return;

  if ( mAssets.at(stringId).path != MemoryResidentPath &&
       ImGui::Button("Reload") )
  {
    unload(stringId);
    load({stringId});
  }

  if ( ImGui::CollapsingHeader("Path", ImGuiTreeNodeFlags_DefaultOpen) )
    ImGui::Text("%s", mAssets.at(stringId).path.string().c_str());

  if ( ImGui::CollapsingHeader("Preview", ImGuiTreeNodeFlags_DefaultOpen) == false )
    return;

  const auto handle = try_get(stringId);

  if ( handle == nullptr )
    return;

  ImGui::Text("%s", handle->c_str());
}

template <>
void
AssetManager <std::string>::ui_show(
  Json::Value& entry ) const
{
  using fmt::format;

  if ( entry.isString() != true )
    jsonValidateArray(entry, AssetJsonDbEntryReference());

  std::string lines {};

  if ( entry.isArray() == false )
    lines.append(entry.asString());
  else
  {
    for ( const auto& line : entry )
      lines.append(line.asString() + '\n');

    if ( lines.empty() == false )
      lines.pop_back();
  }

  if ( ImGui::InputTextMultiline("##multiline", &lines) )
  {
    if ( entry.isArray() == true )
      entry.clear();
    else
      entry = Json::arrayValue;

    const auto linesSeparated = stringSplit(lines, '\n');

    for ( const auto& line : linesSeparated )
      entry.append(line);

    if ( linesSeparated.size() == 1 )
      entry = lines.front();
  }
}

template class
AssetManager <std::string>;

} // namespace cqde::types
