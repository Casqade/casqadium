#include <cqde/types/assets/AssetManager-inl.hpp>

#include <cqde/util/logger.hpp>

#include <json/value.h>


namespace cqde::types
{

template <>
Json::Value
AssetManager <olc::Renderable>::AssetDbReference()
{
  return
  []
  {
    using namespace std::string_literals;

    Json::Value reference = Json::ValueType::objectValue;
    reference.setComment("// texture DB entry must be a JSON object"s,
                         Json::CommentPlacement::commentBefore);

    reference["filter"] = false;
    reference["filter"].setComment("// texture filter mode must be a JSON boolean"s,
                                    Json::CommentPlacement::commentBefore);

    reference["clamp"] = false;
    reference["clamp"].setComment("// texture clamp mode must be a JSON boolean"s,
                                  Json::CommentPlacement::commentBefore);

    reference["path"] = Json::String();
    reference["path"].setComment("// texture path must be a JSON string"s,
                                  Json::CommentPlacement::commentBefore);

    return reference;
  }();
}

template <>
void
AssetManager <olc::Renderable>::parseJsonEntryImpl(
  const Json::Value& entry,
  const AssetId& id )
{
  jsonValidateObject( entry, AssetDbReference() );

  mAssetsProperties[id]["filter"] = entry["filter"];
  mAssetsProperties[id]["clamp"] = entry["clamp"];
}

template <>
AssetManager <olc::Renderable>::AssetHandle
AssetManager <olc::Renderable>::loadImpl(
  const AssetId& id,
  const AssetPath& path ) const
{
  LOG_INFO("Loading texture '{}'", path.string());

  auto texture = std::make_shared <olc::Renderable> ();
  auto sprite = new olc::Sprite();

  const olc::rcode result = sprite->LoadFromFile(path.string());
  switch (result)
  {
    case olc::rcode::OK:
      texture->SetSprite(sprite);
      return texture;

    case olc::rcode::NO_FILE:
      LOG_ERROR("Failed to load texture '{}': file not found",
                path.string());
      return {};

    case olc::rcode::FAIL:
      LOG_ERROR("Failed to load texture '{}': unknown error",
                path.string());
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

