#include <cqde/types/assets/AssetManager.hpp>
#include <cqde/types/assets/AssetManager-inl.hpp>

#include <cqde/util/logger.hpp>

#include <json/value.h>


namespace cqde::types
{

template <>
void
AssetManager <olc::Renderable>::parseJsonEntryImpl(
  const Json::Value& entry,
  const AssetId& id )
{
  if ( entry["filter"].empty() == true )
    throw std::runtime_error("texture filter mode value is undefined");

  if ( entry["filter"].isBool() == false )
    throw std::runtime_error("texture filter mode value type is not a boolean");

  if ( entry["clamp"].empty() == true )
    throw std::runtime_error("texture clamp mode value is undefined");

  if ( entry["clamp"].isBool() == false )
    throw std::runtime_error("texture clamp mode value type is not a boolean");

  mAssetsProperties[id]["filter"] = entry["filter"];
  mAssetsProperties[id]["clamp"] = entry["clamp"];
}

template <>
AssetManager <olc::Renderable>::AssetHandle
AssetManager <olc::Renderable>::loadImpl(
  const AssetId& id,
  const AssetPath& path ) const
{
  LOG_INFO("Loading texture '{}'", path.str());

  auto texture = std::make_shared <olc::Renderable> ();
  auto sprite = new olc::Sprite();

  const olc::rcode result = sprite->LoadFromFile(path.str());
  switch (result)
  {
    case olc::rcode::OK:
      texture->SetSprite(sprite);

      LOG_DEBUG("Loaded texture '{}'", path.str());

      return texture;

    case olc::rcode::NO_FILE:
      LOG_ERROR("Failed to load texture '{}': file not found",
                path.str());
      return {};

    case olc::rcode::FAIL:
      LOG_ERROR("Failed to load texture '{}': unknown error",
                path.str());
      return {};

    default:
      return {};
  }
}

template <>
void
AssetManager <olc::Renderable>::unloadImpl(
  AssetHandle& handle ) const
{
  if ( handle != nullptr && handle->Decal() != nullptr )
    LOG_TRACE("Freeing texture (id {})", handle->Decal()->id);

  handle.reset();
}

template <>
std::shared_ptr <olc::Renderable>
AssetManager <olc::Renderable>::try_get(
  const AssetId& id ) const
{
  std::lock_guard guard(mAssetsMutex);

  switch (status(id))
  {
    case AssetStatus::Loading:
      return {};

    case AssetStatus::Loaded:
    {
      auto texture = mAssets.at(id).handle;

      if ( texture->Decal() != nullptr )
        return texture;

      const AssetPath path = mAssets.at(id).path;

      const bool filter = mAssetsProperties.at(id)["filter"].asBool();
      const bool clamp = mAssetsProperties.at(id)["clamp"].asBool();

      LOG_TRACE("Binding texture '{}' (filtered: {}, clamped: {})",
                id.str(), filter, clamp);

      texture->SetDecal(new olc::Decal(texture->Sprite(), filter, clamp));

      LOG_TRACE("Binded texture '{}' (id: {})",
                id.str(), texture->Decal()->id);

      return texture;
    }

    default:
      return mAssets.at(cqde::null_id).handle;
  }
}

template class
AssetManager <olc::Renderable>;

} // namespace cqde::types

