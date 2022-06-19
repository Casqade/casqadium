#include <cqde/types/assets/AssetManager.hpp>
#include <cqde/types/assets/AssetManager-inl.hpp>

#include <olcPGE/olcPGEX_TTF.hpp>

#include <cqde/util/logger.hpp>

#include <json/value.h>


namespace cqde::types
{

template <>
void
AssetManager <olc::Font>::parseJsonEntryImpl(
  const Json::Value& entry )
{
  if ( entry["size"].empty() == true )
    throw std::runtime_error("font size value is undefined");

  if ( entry["size"].isUInt() == false )
    throw std::runtime_error("font size value type is not an unsigned int");

  mAssetsProperties[entry["path"].asString()]["size"] = entry["size"];
}

template <>
AssetManager <olc::Font>::AssetHandle
AssetManager <olc::Font>::loadImpl(
  const AssetPath& path ) const
{
  const FT_UInt fontSize = mAssetsProperties.at(path)["size"].asUInt();

  LOG_INFO("Loading font '{}' (size {})",
           path.str(), fontSize);

  auto font = std::make_shared <olc::Font> ();
  if ( font->LoadFromFile(path.str(), fontSize) == olc::rcode::OK )
  {
    LOG_DEBUG("Loaded font '{}' (size {})",
             path.str(), fontSize);
    return font;
  }

  LOG_ERROR("Failed to load font '{}' (size {}): {}",
            path.str(), fontSize, font->GetErrorMessage());

  return {};
}

template <>
void
AssetManager <olc::Font>::unloadImpl(
  AssetHandle& handle ) const
{
  LOG_TRACE("Destroying font");
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
      LOG_ASSERT_DEBUG(false, break);
  }

  return {};
}

template class
AssetManager <olc::Font>;

} // namespace cqde::types
