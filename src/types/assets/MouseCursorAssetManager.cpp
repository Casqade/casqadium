#include <cqde/types/assets/AssetManager-inl.hpp>

#include <cqde/logger.hpp>
#include <cqde/json_helpers.hpp>
#include <cqde/render_helpers.hpp>

#include <cqde/types/CasqadiumEngine.hpp>
#include <cqde/types/MouseCursor.hpp>

#include <entt/entity/registry.hpp>

#include <GLFW/glfw3.h>

#include <json/value.h>

#include <imgui.h>
#include <imgui_stdlib.h>


namespace cqde::types
{

template <>
Json::Value
AssetManager <MouseCursor>::AssetJsonDbEntryReference()
{
  using ValueType = Json::ValueType;
  using namespace std::string_literals;

  Json::Value reference = ValueType::objectValue;
  reference.setComment("// mouse cursor DB entry must be a JSON object"s,
                       Json::CommentPlacement::commentBefore);

  reference["flipHorizontal"] = ValueType::booleanValue;
  reference["flipHorizontal"].setComment("// mouse cursor 'flipHorizontal' must be a JSON boolean"s,
                                          Json::CommentPlacement::commentBefore);

  reference["flipVertical"] = ValueType::booleanValue;
  reference["flipVertical"].setComment("// mouse cursor 'flipVertical' must be a JSON boolean"s,
                                        Json::CommentPlacement::commentBefore);

  reference["path"] = ValueType::stringValue;
  reference["path"].setComment("// mouse cursor 'path' must be a JSON string"s,
                                Json::CommentPlacement::commentBefore);

  return reference;
}

template <>
void
AssetManager <MouseCursor>::Validate(
  const Json::Value& json )
{
  using ValueType = Json::ValueType;
  using namespace std::string_literals;

  Json::Value reference = ValueType::objectValue;
  reference.setComment("// mouse cursor DB must be a JSON object"s,
                       Json::CommentPlacement::commentBefore);

  Json::Value& asset = reference["cqde_json_anykey"];
  asset = AssetJsonDbEntryReference();

  jsonValidateObject(json, reference);
}

template <>
void
AssetManager <MouseCursor>::initPlaceholders()
{
  using namespace cqde::literals;

  olc::Sprite sprite {16, 16};

  for ( int32_t px = 0;
        px < sprite.width;
        ++px )
    for ( int32_t py = 0;
          py < sprite.height;
          ++py )
    sprite.SetPixel(
      px, py,
      (px + py) % 2 == 0 ? olc::BLACK : olc::MAGENTA );

  const auto cursorNull = std::make_shared <MouseCursor> ();
  cursorNull->setSprite(std::move(sprite));
  cursorNull->create(0, 0);

  insert(null_id, cursorNull);

  sprite = spriteFromText("ERROR", olc::RED, olc::BLANK, true);

  const auto cursorError = std::make_shared <MouseCursor> ();
  cursorError->setSprite(sprite);
  cursorError->create(0, 0);

  insert("error"_id, cursorError);
}

template <>
void
AssetManager <MouseCursor>::parseJsonEntryImpl(
  const Json::Value& entry,
  const AssetId& id )
{
  jsonValidateObject( entry, AssetJsonDbEntryReference() );

  mAssetsProperties[id]["flipHorizontal"] = entry["flipHorizontal"];
  mAssetsProperties[id]["flipVertical"] = entry["flipVertical"];

  mAssetsProperties[id]["hotspotX"] = entry["hotspotX"];
  mAssetsProperties[id]["hotspotY"] = entry["hotspotY"];
}

template <>
AssetManager <MouseCursor>::AssetHandle
AssetManager <MouseCursor>::loadImpl(
  const AssetId& id,
  const AssetPath& path ) const
{
  using Flip = olc::Sprite::Flip;

  const bool flipH = mAssetsProperties.at(id)["flipHorizontal"].asBool();
  const bool flipV = mAssetsProperties.at(id)["flipVertical"].asBool();

  LOG_INFO("Loading mouse cursor '{}'", path.string());

  olc::Sprite sprite {};

  const olc::rcode result = sprite.LoadFromFile(path.string());

  switch (result)
  {
    case olc::rcode::OK:
    {
      uint8_t flip {};

      if ( flipH == true )
        flip |= Flip::HORIZ;

      if ( flipV == true )
        flip |= Flip::VERT;

      sprite.SetFlipped(static_cast <Flip> (flip));

      const auto cursor = std::make_shared <MouseCursor> ();
      cursor->setSprite(std::move(sprite));

      return cursor;
    }

    case olc::rcode::NO_FILE:
      LOG_ERROR("Failed to load mouse cursor '{}': file not found",
                path.string());
      return {};

    case olc::rcode::FAIL:
      LOG_ERROR("Failed to load mouse cursor '{}': unknown error",
                path.string());
      return {};

    default:
      return {};
  }
}

template <>
void
AssetManager <MouseCursor>::unloadImpl(
  AssetHandle& handle ) const
{
  handle.reset();
}

template <>
std::shared_ptr <MouseCursor>
AssetManager <MouseCursor>::try_get(
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
      auto cursor = mAssets.at(id).handle;

      if ( static_cast <GLFWcursor*> (*cursor) != nullptr )
        return cursor;

      const int xhot = mAssetsProperties.at(id)["hotspotX"].asInt();
      const int yhot = mAssetsProperties.at(id)["hotspotY"].asInt();

      LOG_TRACE("Creating mouse cursor '{}'", id.str());

      cursor->create(xhot, yhot);

      return mAssets.at(id).handle;
    }

    default:
      return mAssets.at(null_id).handle;
  }
}

template <>
void
AssetManager <MouseCursor>::ui_show_preview(
  const AssetId& cursorId,
  entt::registry& registry )
{
  using fmt::format;

  const auto assetStatus = status(cursorId);

  if ( ImGui::CollapsingHeader("Status", ImGuiTreeNodeFlags_DefaultOpen) )
    ImGui::TextColored(ImGui::ColorConvertU32ToFloat4(StatusAsColor(assetStatus)),
                       "%s", StatusAsString(assetStatus).c_str());

  if ( assetStatus == AssetStatus::Undefined )
    return;

  if ( mAssets.at(cursorId).path != MemoryResidentPath )
  {
    ImGui::SameLine();
    if ( ImGui::Button("Reload") )
    {
      unload({cursorId});
      load({cursorId});
    }
  }

  if ( ImGui::CollapsingHeader("Path", ImGuiTreeNodeFlags_DefaultOpen) )
    ImGui::Text("%s", mAssets.at(cursorId).path.string().c_str());

  const auto handle = try_get(cursorId);

  if ( handle == nullptr )
    return;

  if ( ImGui::CollapsingHeader("Preview", ImGuiTreeNodeFlags_DefaultOpen) == false )
    return;

  if ( ImGui::Button("Set cursor") )
  {
    const auto engine = registry.ctx().get <CasqadiumEngine*> ();
    glfwSetCursor(engine->window(), *handle);
  }

  if ( ImGui::Button("Reset cursor") )
  {
    const auto engine = registry.ctx().get <CasqadiumEngine*> ();
    glfwSetCursor(engine->window(), {});
  }

  static AssetId cursorCurrent {cursorId};
  static std::unique_ptr <olc::Decal> cursorPreview {nullptr};

  if ( cursorPreview == nullptr ||
       cursorCurrent != cursorId )
  {
    cursorCurrent = cursorId;
    cursorPreview = std::make_unique <olc::Decal> (&handle->sprite());
  }

  if ( cursorPreview == nullptr ||
       cursorPreview->id == 0 )
    return;

  float spriteRatio = 1.0f * cursorPreview->sprite->width / cursorPreview->sprite->height;

  float height = ImGui::GetContentRegionAvail().y / 2.0f;
  float width = height * spriteRatio;

  ImGui::Image(ImTextureID(cursorPreview->id), {width, height});
}

template <>
void
AssetManager <MouseCursor>::ui_show(
  Json::Value& entry ) const
{
  if ( ImGui::CollapsingHeader("Hotspot", ImGuiTreeNodeFlags_DefaultOpen) )
  {
    ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);

    int xhot = entry["hotspotX"].asInt();
    if ( ImGui::DragInt("##hotspotX", &xhot, 0.1f, 0, 0, "X: %d") )
      entry["hotspotX"] = xhot;

    int yhot = entry["hotspotY"].asInt();
    if ( ImGui::DragInt("##hotspotY", &yhot, 0.1f, 0, 0, "Y: %d") )
      entry["hotspotY"] = yhot;

    ImGui::PopItemWidth();
  }

  if ( ImGui::CollapsingHeader("Flip", ImGuiTreeNodeFlags_DefaultOpen) )
  {
    bool flipH = entry["flipHorizontal"].asBool();
    if ( ImGui::Checkbox("Flip horizontally##flipH", &flipH) )
      entry["flipHorizontal"] = flipH;

    bool flipV = entry["flipVertical"].asBool();
    if ( ImGui::Checkbox("Flip vertically##flipV", &flipV) )
      entry["flipVertical"] = flipV;
  }

  if ( ImGui::CollapsingHeader("Path", ImGuiTreeNodeFlags_DefaultOpen) )
  {
    std::string path = entry["path"].asString();
    if ( ImGui::InputText("##mouseCursorPath", &path) )
      entry["path"] = path;
  }
}

template class
AssetManager <MouseCursor>;

} // namespace cqde::types

