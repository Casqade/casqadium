#include <cqde/types/assets/AssetManager-inl.hpp>

#include <olcPGE/olcPGEX_TTF.hpp>

#include <cqde/json_helpers.hpp>

#include <cqde/util/logger.hpp>

#include <json/value.h>

#include <imgui.h>
#include <imgui_stdlib.h>


namespace cqde::types
{

template <>
Json::Value
AssetManager <olc::Font>::AssetJsonDbEntryReference()
{
  using ValueType = Json::ValueType;
  using namespace std::string_literals;

  Json::Value reference = ValueType::objectValue;
  reference.setComment("// font DB entry must be a JSON object"s,
                       Json::CommentPlacement::commentBefore);

  reference["size"] = 1u;
  reference["size"].setComment("// font size must be a JSON unsigned integer"s,
                                Json::CommentPlacement::commentBefore);

  reference["path"] = ValueType::stringValue;
  reference["path"].setComment("// font path must be a JSON string"s,
                                Json::CommentPlacement::commentBefore);

  return reference;
}

template <>
void
AssetManager <olc::Font>::Validate(
  const Json::Value& json )
{
  using ValueType = Json::ValueType;
  using namespace std::string_literals;

  Json::Value reference = ValueType::objectValue;
  reference.setComment("// font DB must be a JSON object"s,
                       Json::CommentPlacement::commentBefore);

  Json::Value& asset = reference["cqde_json_anykey"];
  asset = AssetJsonDbEntryReference();

  jsonValidateObject(json, reference);
}

template <>
void
AssetManager <olc::Font>::initPlaceholders()
{
  auto font = std::make_shared <olc::Font> ();

  if ( font->LoadFontDefault(16) != olc::rcode::OK )
  {
    LOG_ERROR("Failed to create default font: {}", font->GetErrorMessage());
    CQDE_ASSERT_DEBUG(false, font.reset());
  }

  insert(null_id, font);
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

  const auto font = std::make_shared <olc::Font> ();
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
    case AssetStatus::Loaded:
      return mAssets.at(id).handle;

    default:
      return mAssets.at(null_id).handle;
  }
}

template <>
void
AssetManager <olc::Font>::ui_show_preview(
  const AssetId& fontId,
  entt::registry& )
{
  using fmt::format;

  const auto assetStatus = status(fontId);

  if ( ImGui::CollapsingHeader("Status", ImGuiTreeNodeFlags_DefaultOpen) )
    ImGui::TextColored(ImGui::ColorConvertU32ToFloat4(StatusAsColor(assetStatus)),
                       "%s", StatusAsString(assetStatus).c_str());

  if ( assetStatus == AssetStatus::Undefined )
    return;

  if ( mAssets.at(fontId).path != MemoryResidentPath )
  {
    ImGui::SameLine();
    if ( ImGui::Button("Reload") )
    {
      unload({fontId});
      load({fontId});
    }
  }

  if ( ImGui::CollapsingHeader("Path", ImGuiTreeNodeFlags_DefaultOpen) )
    ImGui::Text("%s", mAssets.at(fontId).path.string().c_str());

  const auto handle = try_get(fontId);

  if ( handle == nullptr )
    return;

  if ( ImGui::CollapsingHeader("Size", ImGuiTreeNodeFlags_DefaultOpen) )
    ImGui::Text("%s", format("{}", handle->GetFontSize()).c_str());

  if ( ImGui::CollapsingHeader("Preview", ImGuiTreeNodeFlags_DefaultOpen) == false )
    return;

  static AssetId fontCurrent {fontId};
  static std::string textSample {u8"Preview"};
  static std::unique_ptr <olc::Decal> decalPreview {nullptr};

  if ( ImGui::InputTextWithHint("##textPreview", "Type text to preview", &textSample) ||
       decalPreview == nullptr ||
       fontCurrent != fontId )
  {
    if ( textSample.empty() == false )
    {
      fontCurrent = fontId;
      std::wstring_convert<std::codecvt_utf8 <char32_t>, char32_t> convert {};
      decalPreview.reset(handle->RenderStringToDecal(convert.from_bytes(textSample), olc::WHITE, false));
    }
  }

  if ( decalPreview == nullptr ||
       decalPreview->id < 0 )
    return;

  float spriteRatio = 1.0f * decalPreview->sprite->width / decalPreview->sprite->height;

  float height = ImGui::GetContentRegionAvail().y / 2.0f;
  float width = height * spriteRatio;

  ImGui::Image(ImTextureID(decalPreview->id), {width, height});
}

template <>
void
AssetManager <olc::Font>::ui_show(
  Json::Value& entry ) const
{
  if ( ImGui::CollapsingHeader("Size", ImGuiTreeNodeFlags_DefaultOpen) )
  {
    int32_t size = entry["size"].asUInt();
    if ( ImGui::DragInt("##fontSize", &size,
                        1.0f, 1, 500, "%d", ImGuiSliderFlags_AlwaysClamp) )
      entry["size"] = size;
  }

  if ( ImGui::CollapsingHeader("Path", ImGuiTreeNodeFlags_DefaultOpen) )
  {
    std::string path = entry["path"].asString();
    if ( ImGui::InputText("##fontPath", &path) )
      entry["path"] = path;
  }
}

template class
AssetManager <olc::Font>;

} // namespace cqde::types
