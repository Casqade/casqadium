#include <cqde/types/assets/AssetManager-inl.hpp>

#include <cqde/json_helpers.hpp>
#include <cqde/render_helpers.hpp>

#include <cqde/util/logger.hpp>

#include <olcPGE/olcPixelGameEngine.hpp>

#include <json/value.h>

#include <imgui.h>
#include <imgui_stdlib.h>


namespace cqde::types
{

template <>
Json::Value
AssetManager <olc::Renderable>::AssetJsonDbEntryReference()
{
  using ValueType = Json::ValueType;
  using namespace std::string_literals;

  Json::Value reference = ValueType::objectValue;
  reference.setComment("// texture DB entry must be a JSON object"s,
                       Json::CommentPlacement::commentBefore);

  reference["filter"] = ValueType::booleanValue;
  reference["filter"].setComment("// texture filter mode must be a JSON boolean"s,
                                  Json::CommentPlacement::commentBefore);

  reference["clamp"] = ValueType::booleanValue;
  reference["clamp"].setComment("// texture clamp mode must be a JSON boolean"s,
                                Json::CommentPlacement::commentBefore);

  reference["path"] = ValueType::stringValue;
  reference["path"].setComment("// texture path must be a JSON string"s,
                                Json::CommentPlacement::commentBefore);

  return reference;
}

template <>
void
AssetManager <olc::Renderable>::Validate(
  const Json::Value& json )
{
  using ValueType = Json::ValueType;
  using namespace std::string_literals;

  Json::Value reference = ValueType::objectValue;
  reference.setComment("// texture DB must be a JSON object"s,
                       Json::CommentPlacement::commentBefore);

  Json::Value& asset = reference["cqde_json_anykey"];
  asset = AssetJsonDbEntryReference();

  jsonValidateObject(json, reference);
}

template <>
void
AssetManager <olc::Renderable>::initPlaceholders()
{
  using namespace cqde::literals;

  const auto textureNull = std::make_shared <olc::Renderable> ();
  textureNull->Create(16, 16);

  for ( int32_t px = 0;
        px < textureNull->Sprite()->width;
        ++px )
    for ( int32_t py = 0;
          py < textureNull->Sprite()->height;
          ++py )
    textureNull->Sprite()->SetPixel(px, py,
                                (px + py) % 2 == 0 ? olc::BLACK : olc::MAGENTA);

  textureNull->Decal()->Update();

  insert(null_id, textureNull);

  const auto textureError = textureFromText("ERROR", olc::RED, olc::BLANK, true);
  insert("error"_id, textureError);
}

template <>
void
AssetManager <olc::Renderable>::parseJsonEntryImpl(
  const Json::Value& entry,
  const AssetId& id )
{
  jsonValidateObject( entry, AssetJsonDbEntryReference() );

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
  else
    LOG_TRACE("Freeing unbinded texture");

  handle.reset();
}

template <>
std::shared_ptr <olc::Renderable>
AssetManager <olc::Renderable>::try_get(
  const AssetId& id ) const
{
  using namespace cqde::literals;

  std::lock_guard guard(mAssetsMutex);

  switch (status(id))
  {
    case AssetStatus::Error:
      return mAssets.at("error"_id).handle;

    case AssetStatus::Loading:
      return {};

    case AssetStatus::Loaded:
    {
      auto texture = mAssets.at(id).handle;

      if ( texture->Decal() != nullptr )
        return texture;

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
      return mAssets.at(null_id).handle;
  }
}

template <>
void
AssetManager <olc::Renderable>::ui_show_preview(
  const AssetId& textureId,
  entt::registry& )
{
  using fmt::format;

  const auto assetStatus = status(textureId);

  if ( ImGui::CollapsingHeader("Status", ImGuiTreeNodeFlags_DefaultOpen) )
    ImGui::TextColored(ImGui::ColorConvertU32ToFloat4(StatusAsColor(assetStatus)),
                       "%s", StatusAsString(assetStatus).c_str());

  if ( assetStatus == AssetStatus::Undefined )
    return;

  if ( mAssets.at(textureId).path != MemoryResidentPath )
  {
    ImGui::SameLine();
    if ( ImGui::Button("Reload") )
    {
      unload({textureId});
      load({textureId});
    }
  }

  if ( ImGui::CollapsingHeader("Path", ImGuiTreeNodeFlags_DefaultOpen) )
    ImGui::Text("%s", mAssets.at(textureId).path.string().c_str());

  const auto handle = try_get(textureId);

  if ( handle == nullptr ||
       handle->Decal() == nullptr )
    return;

  if ( ImGui::CollapsingHeader("Size", ImGuiTreeNodeFlags_DefaultOpen) )
    ImGui::Text("%s", format("{}x{}", handle->Sprite()->width,
                                      handle->Sprite()->height).c_str());

  if ( ImGui::CollapsingHeader("Preview", ImGuiTreeNodeFlags_DefaultOpen) == false )
    return;

  const int32_t decalId = handle->Decal()->id;

  if ( decalId < 0 )
    return;

  float width = ImGui::GetContentRegionAvail().x;
  float height = ImGui::GetContentRegionAvail().y;

  const float spriteRatio = 1.0f * handle->Sprite()->width
                          / handle->Sprite()->height;

  const float screenRatio = width / height;

  width = std::min(width, width / screenRatio * spriteRatio);
  height = std::min(height, height * screenRatio / spriteRatio);

  ImGui::Image(ImTextureID(decalId), {width, height});
}

template <>
void
AssetManager <olc::Renderable>::ui_show(
  Json::Value& entry ) const
{
  if ( ImGui::CollapsingHeader("Clamp", ImGuiTreeNodeFlags_DefaultOpen) )
  {
    bool clamp = entry["clamp"].asBool();
    if ( ImGui::Checkbox("##textureClamp", &clamp) )
      entry["clamp"] = clamp;
  }

  if ( ImGui::CollapsingHeader("Filter", ImGuiTreeNodeFlags_DefaultOpen) )
  {
    bool filter = entry["filter"].asBool();
    if ( ImGui::Checkbox("##textureFilter", &filter) )
      entry["filter"] = filter;
  }

  if ( ImGui::CollapsingHeader("Path", ImGuiTreeNodeFlags_DefaultOpen) )
  {
    std::string path = entry["path"].asString();
    if ( ImGui::InputText("##texturePath", &path) )
      entry["path"] = path;
  }
}

template class
AssetManager <olc::Renderable>;

} // namespace cqde::types

