#include <cqde/types/assets/AssetManager-inl.hpp>

#include <cqde/json_helpers.hpp>
#include <cqde/render_helpers.hpp>

#include <cqde/util/logger.hpp>

#include <json/value.h>

#include <soloud.h>

#include <imgui.h>
#include <imgui_stdlib.h>


const static std::map <SoLoud::SOLOUD_ERRORS, std::string> SoLoudErrorStrings
{
  {SoLoud::SOLOUD_ERRORS::SO_NO_ERROR, "SO_NO_ERROR"},
  {SoLoud::SOLOUD_ERRORS::INVALID_PARAMETER, "INVALID_PARAMETER"},
  {SoLoud::SOLOUD_ERRORS::FILE_NOT_FOUND, "FILE_NOT_FOUND"},
  {SoLoud::SOLOUD_ERRORS::FILE_LOAD_FAILED, "FILE_LOAD_FAILED"},
  {SoLoud::SOLOUD_ERRORS::DLL_NOT_FOUND, "DLL_NOT_FOUND"},
  {SoLoud::SOLOUD_ERRORS::OUT_OF_MEMORY, "OUT_OF_MEMORY"},
  {SoLoud::SOLOUD_ERRORS::NOT_IMPLEMENTED, "NOT_IMPLEMENTED"},
  {SoLoud::SOLOUD_ERRORS::UNKNOWN_ERROR, "UNKNOWN_ERROR"},
};

namespace cqde::types
{

template <>
Json::Value
AssetManager <SoLoud::AudioSource>::AssetJsonDbEntryReference()
{
  using ValueType = Json::ValueType;
  using namespace std::string_literals;

  Json::Value reference = ValueType::objectValue;
  reference.setComment("// audio DB entry must be a JSON object"s,
                       Json::CommentPlacement::commentBefore);

  reference["type"] = ValueType::stringValue;
  reference["type"].setComment("// audio type must be a JSON string"s,
                                Json::CommentPlacement::commentBefore);

  reference["path"] = ValueType::stringValue;
  reference["path"].setComment("// audio path must be a JSON string"s,
                                Json::CommentPlacement::commentBefore);

  return reference;
}

template <>
void
AssetManager <SoLoud::AudioSource>::Validate(
  const Json::Value& json )
{
  using ValueType = Json::ValueType;
  using namespace std::string_literals;

  Json::Value reference = ValueType::objectValue;
  reference.setComment("// audio DB must be a JSON object"s,
                       Json::CommentPlacement::commentBefore);

  Json::Value& asset = reference["cqde_json_anykey"];
  asset = AssetJsonDbEntryReference();

  jsonValidateObject(json, reference);
}

template <>
void
AssetManager <SoLoud::AudioSource>::initPlaceholders()
{
  insert(null_id, std::make_shared <SoLoud::AudioSource> ());
}

template <>
void
AssetManager <SoLoud::AudioSource>::parseJsonEntryImpl(
  const Json::Value& entry,
  const AssetId& id )
{
  jsonValidateObject( entry, AssetJsonDbEntryReference() );

  mAssetsProperties[id]["type"] = entry["type"];
}

template <>
AssetManager <SoLoud::AudioSource>::AssetHandle
AssetManager <SoLoud::AudioSource>::loadImpl(
  const AssetId& id,
  const AssetPath& path ) const
{
  LOG_INFO("Loading audio '{}'", path.string());

  const auto audio = std::make_shared <SoLoud::AudioSource> ();

//  todo: impl

  return audio;
}

template <>
void
AssetManager <SoLoud::AudioSource>::unloadImpl(
  AssetHandle& handle ) const
{
  handle.reset();
}

template <>
void
AssetManager <SoLoud::AudioSource>::ui_show_preview(
  const AssetId& audioId )
{
  using fmt::format;

  const auto assetStatus = status(audioId);

  if ( ImGui::CollapsingHeader("Status", ImGuiTreeNodeFlags_DefaultOpen) )
    ImGui::TextColored(ImGui::ColorConvertU32ToFloat4(StatusAsColor(assetStatus)),
                       "%s", StatusAsString(assetStatus).c_str());

  if ( assetStatus == AssetStatus::Undefined )
    return;

  if ( mAssets.at(audioId).path != MemoryResidentPath )
  {
    ImGui::SameLine();
    if ( ImGui::Button("Reload") )
    {
      unload({audioId});
      load({audioId});
    }
  }

  if ( ImGui::CollapsingHeader("Path", ImGuiTreeNodeFlags_DefaultOpen) )
    ImGui::Text("%s", mAssets.at(audioId).path.string().c_str());

  if ( ImGui::CollapsingHeader("Preview", ImGuiTreeNodeFlags_DefaultOpen) == false )
    return;

  const auto handle = try_get(audioId);

  if ( handle == nullptr )
    return;
}

template <>
void
AssetManager <SoLoud::AudioSource>::ui_show(
  Json::Value& entry ) const
{
  if ( ImGui::CollapsingHeader("Type", ImGuiTreeNodeFlags_DefaultOpen) )
  {
    auto type = entry["type"].asString();
    if ( ImGui::InputText("##audioType", &type) )
      entry["type"] = type;
  }

  if ( ImGui::CollapsingHeader("Path", ImGuiTreeNodeFlags_DefaultOpen) )
  {
    std::string path = entry["path"].asString();
    if ( ImGui::InputText("##audioPath", &path) )
      entry["path"] = path;
  }
}

template class
AssetManager <SoLoud::AudioSource>;

} // namespace cqde::types

