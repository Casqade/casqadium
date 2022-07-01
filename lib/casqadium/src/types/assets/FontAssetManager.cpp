#include <cqde/types/assets/AssetManager-inl.hpp>

#include <olcPGE/olcPGEX_TTF.hpp>

#include <cqde/util/logger.hpp>

#include <json/value.h>


namespace cqde::types
{

template <>
Json::Value
AssetManager <olc::Font>::AssetJsonDbEntryReference()
{
  return
  []
  {
    using ValueType = Json::ValueType;
    using namespace std::string_literals;

    Json::Value reference = ValueType::objectValue;
    reference.setComment("// font DB entry must be a JSON object"s,
                         Json::CommentPlacement::commentBefore);

    reference["size"] = ValueType::uintValue;
    reference["size"].setComment("// font size must be a JSON unsigned integer"s,
                                    Json::CommentPlacement::commentBefore);

    reference["path"] = ValueType::stringValue;
    reference["path"].setComment("// font path must be a JSON string"s,
                                  Json::CommentPlacement::commentBefore);

    return reference;
  }();
}

template <>
void
AssetManager <olc::Font>::parseJsonEntryImpl(
  const Json::Value& entry,
  const AssetId& id )
{
  jsonValidateObject(entry, AssetJsonDbEntryReference());

  mAssetsProperties[id.str()]["size"] = entry["size"];
}

template <>
AssetManager <olc::Font>::AssetHandle
AssetManager <olc::Font>::loadImpl(
  const AssetId& id,
  const AssetPath& path ) const
{
  const FT_UInt fontSize = mAssetsProperties.at(id)["size"].asUInt();

  LOG_INFO("Loading font '{}' (size {})",
           path.string(), fontSize);

  auto font = std::make_shared <olc::Font> ();
  if ( font->LoadFromFile(path.string(), fontSize) == olc::rcode::OK )
    return font;

  LOG_ERROR("Failed to load font '{}' (size {}): {}",
            path.string(), fontSize, font->GetErrorMessage());

  return {};
}

template <>
void
AssetManager <olc::Font>::unloadImpl(
  AssetHandle& handle ) const
{
  LOG_TRACE("Freeing font");
  handle.reset();
}

template <>
std::shared_ptr <olc::Font>
AssetManager <olc::Font>::try_get(
  const AssetId& id ) const
{
  std::lock_guard guard(mAssetsMutex);

  switch (status(id))
  {
//    case AssetStatus::Loading:
//      break;

    case AssetStatus::Loaded:
      return mAssets.at(id).handle;

    default:
      CQDE_ASSERT_DEBUG(false, break);
  }

  return {};
}

template class
AssetManager <olc::Font>;

} // namespace cqde::types
