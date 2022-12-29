#include <cqde/types/assets/AssetManager-inl.hpp>

#include <cqde/json_helpers.hpp>
#include <cqde/render_helpers.hpp>

#include <cqde/util/logger.hpp>

#include <entt/entity/registry.hpp>

#include <json/value.h>

#include <soloud.h>
#include <soloud_wav.h>
#include <soloud_wavstream.h>
#include <soloud_modplug.h>
#include <soloud_openmpt.h>
#include <soloud_monotone.h>
#include <soloud_sfxr.h>
#include <soloud_speech.h>
#include <soloud_tedsid.h>
#include <soloud_vizsn.h>
#include <soloud_noise.h>

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

enum class AudioSourceType
{
  EnumBegin,
  Wav = EnumBegin,
  WavStream,
  Modplug,
  Monotone,
  Noise,
  Openmpt,
  Sfxr,
  Speech,
  TedSid,
  Vizsn,

  EnumEnd = Vizsn,
};

std::string
AudioSourceTypeToString(
  const AudioSourceType type )
{
  switch (type)
  {
    case AudioSourceType::Wav:
      return "wav";

    case AudioSourceType::WavStream:
      return "wavstream";

    case AudioSourceType::Modplug:
      return "modplug";

    case AudioSourceType::Monotone:
      return "monotone";

    case AudioSourceType::Noise:
      return "noise";

    case AudioSourceType::Openmpt:
      return "openmpt";

    case AudioSourceType::Sfxr:
      return "sfxr";

    case AudioSourceType::Speech:
      return "speech";

    case AudioSourceType::TedSid:
      return "tedsid";

    case AudioSourceType::Vizsn:
      return "vizsn";
  }

  CQDE_ASSERT_DEBUG(false, return "");
}

AudioSourceType
AudioSourceTypeFromString(
  const std::string& type )
{
  using fmt::format;

  if ( type == AudioSourceTypeToString(AudioSourceType::Wav) )
    return AudioSourceType::Wav;

  if ( type == AudioSourceTypeToString(AudioSourceType::WavStream) )
    return AudioSourceType::WavStream;

  if ( type == AudioSourceTypeToString(AudioSourceType::Modplug) )
    return AudioSourceType::Modplug;

  if ( type == AudioSourceTypeToString(AudioSourceType::Monotone) )
    return AudioSourceType::Monotone;

  if ( type == AudioSourceTypeToString(AudioSourceType::Noise) )
    return AudioSourceType::Noise;

  if ( type == AudioSourceTypeToString(AudioSourceType::Openmpt) )
    return AudioSourceType::Openmpt;

  if ( type == AudioSourceTypeToString(AudioSourceType::Sfxr) )
    return AudioSourceType::Sfxr;

  if ( type == AudioSourceTypeToString(AudioSourceType::Speech) )
    return AudioSourceType::Speech;

  if ( type == AudioSourceTypeToString(AudioSourceType::TedSid) )
    return AudioSourceType::TedSid;

  if ( type == AudioSourceTypeToString(AudioSourceType::Vizsn) )
    return AudioSourceType::Vizsn;

  throw std::runtime_error(
    format("'{}' is not a valid audio source type", type));
}


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

  reference["type"] = AudioSourceTypeToString(AudioSourceType::Wav);
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
  insert(null_id, std::make_shared <SoLoud::Wav> ());
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
  using SoLoud::SOLOUD_ERRORS;
  using SoLoud::WAVSTREAM_FILETYPE;

  AssetHandle audio {};
  SoLoud::result result {};

  const auto type = mAssetsProperties.at(id)["type"].asString();

  switch ( AudioSourceTypeFromString(type) )
  {
    case AudioSourceType::Wav:
    {
      LOG_INFO("Loading wav audio '{}'", path.string());

      audio = std::make_shared <SoLoud::Wav> ();
      const auto wav = std::dynamic_pointer_cast <SoLoud::Wav> (audio);

      result = wav->load(path.string().c_str());

      if ( result == SOLOUD_ERRORS::SO_NO_ERROR )
        return audio;

      break;
    }

    case AudioSourceType::WavStream:
    {
      LOG_INFO("Loading wavstream audio '{}'", path.string());

      audio = std::make_shared <SoLoud::WavStream> ();
      const auto stream = std::dynamic_pointer_cast <SoLoud::WavStream> (audio);

      result = stream->load(path.string().c_str());

      if ( result == SOLOUD_ERRORS::SO_NO_ERROR )
        return audio;

      break;
    }

    case AudioSourceType::Modplug:
    {
      LOG_INFO("Loading modplug audio '{}'", path.string());

      audio = std::make_shared <SoLoud::Modplug> ();
      const auto modplug = std::dynamic_pointer_cast <SoLoud::Modplug> (audio);

      result = modplug->load(path.string().c_str());

      if ( result == SOLOUD_ERRORS::SO_NO_ERROR )
        return audio;

      break;
    }

    case AudioSourceType::Monotone:
    {
      LOG_INFO("Loading monotone audio '{}'", path.string());

      audio = std::make_shared <SoLoud::Monotone> ();
      const auto monotone = std::dynamic_pointer_cast <SoLoud::Monotone> (audio);

      result = monotone->load(path.string().c_str());

      if ( result == SOLOUD_ERRORS::SO_NO_ERROR )
        return audio;

//      todo: monotone->setParams ?

      break;
    }

    case AudioSourceType::Noise:
    {
      LOG_INFO("Loading noise audio '{}'", path.string());

      audio = std::make_shared <SoLoud::Noise> ();
      const auto noise = std::dynamic_pointer_cast <SoLoud::Noise> (audio);

//      todo: noise->setType ?
//      todo: noise->setOctaveScale ?

      break;
    }

    case AudioSourceType::Openmpt:
    {
      LOG_INFO("Loading openmpt audio '{}'", path.string());

      audio = std::make_shared <SoLoud::Openmpt> ();
      const auto modplug = std::dynamic_pointer_cast <SoLoud::Openmpt> (audio);

      result = modplug->load(path.string().c_str());

      if ( result == SOLOUD_ERRORS::SO_NO_ERROR )
        return audio;

      break;
    }

    case AudioSourceType::Sfxr:
    {
      LOG_INFO("Loading Sfxr audio '{}'", path.string());

      audio = std::make_shared <SoLoud::Sfxr> ();
      const auto sfxr = std::dynamic_pointer_cast <SoLoud::Sfxr> (audio);

      result = sfxr->loadParams(path.string().c_str());

      if ( result == SOLOUD_ERRORS::SO_NO_ERROR )
        return audio;

//      todo: sfxr->loadPreset ?

      return audio;
    }

    case AudioSourceType::Speech:
    {
      LOG_INFO("Loading Klatt speech audio '{}'", path.string());

      audio = std::make_shared <SoLoud::Speech> ();
      const auto speech = std::dynamic_pointer_cast <SoLoud::Speech> (audio);

//      todo: speech->setText ?
//      todo: speech->setParams ?

      return audio;
    }

    case AudioSourceType::TedSid:
    {
      LOG_INFO("Loading TED/SID audio '{}'", path.string());

      audio = std::make_shared <SoLoud::TedSid> ();
      const auto tedsid = std::dynamic_pointer_cast <SoLoud::TedSid> (audio);

      result = tedsid->load(path.string().c_str());

      if ( result == SOLOUD_ERRORS::SO_NO_ERROR )
        return audio;

      return audio;
    }

    case AudioSourceType::Vizsn:
    {
      LOG_INFO("Loading Vizsn speech audio '{}'", path.string());

      audio = std::make_shared <SoLoud::Vizsn> ();
      const auto vizsn = std::dynamic_pointer_cast <SoLoud::Vizsn> (audio);

//      todo: vizsn->setText ?

      return audio;
    }

    default:
    {
      LOG_ERROR("Failed to load audio '{}': unknown audio type '{}'",
                path.string(), type);

      return {};
    }
  }

  const SOLOUD_ERRORS errorCode = static_cast <SOLOUD_ERRORS> (result);

  if ( SoLoudErrorStrings.count(errorCode) > 0 )
    LOG_ERROR("Failed to load audio '{}': {}",
              id.str(), SoLoudErrorStrings.at(errorCode));
  else
    LOG_ERROR("Failed to load audio '{}': unknown error", id.str());

  return {};
}

template <>
void
AssetManager <SoLoud::AudioSource>::unloadImpl(
  AssetHandle& handle ) const
{
  LOG_TRACE("Freeing audio");
  handle.reset();
}

template <>
void
AssetManager <SoLoud::AudioSource>::ui_show_preview(
  const AssetId& audioId,
  entt::registry& registry )
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

  if ( handle == nullptr ||
       audioId == null_id )
    return ImGui::Text("No data to play");

  static auto audioPrev = audioId;

  auto& soloud = registry.ctx().at <SoLoud::Soloud> ();

  static float audioVolume {1.0f};
  static float audioSpeed {1.0f};
  static bool loop {};
  const static SoLoud::handle voiceHandleInvalid {0xfffff000};
  static SoLoud::handle voiceHandle {voiceHandleInvalid};

  if ( audioPrev != audioId )
  {
    if ( soloud.isValidVoiceHandle(voiceHandle) == true )
      soloud.stop(voiceHandle);

    voiceHandle = voiceHandleInvalid;
    audioPrev = audioId;
  }

  if ( ImGui::Button(">##audioPlay") )
  {
    if ( soloud.isValidVoiceHandle(voiceHandle) == true )
    {
      if ( soloud.getPause(voiceHandle) == true )
        soloud.setPause(voiceHandle, false);
      else
      {
        soloud.stop(voiceHandle);
        voiceHandle = soloud.play(*handle, audioVolume);
      }
    }
    else
      voiceHandle = soloud.play(*handle, audioVolume);

    soloud.setLooping(voiceHandle, loop);
    soloud.setVolume(voiceHandle, audioVolume);
    soloud.setRelativePlaySpeed(voiceHandle, audioSpeed);
  }

  ImGui::BeginDisabled(soloud.isValidVoiceHandle(voiceHandle) == false);
  ImGui::SameLine();

  if ( ImGui::Button("||##audioPause") )
    soloud.setPause(voiceHandle, true);

  ImGui::EndDisabled();

  ImGui::BeginDisabled(soloud.isValidVoiceHandle(voiceHandle) == false);
  ImGui::SameLine();

  if ( ImGui::Button("[]##audioStop") )
  {
    soloud.stop(voiceHandle);
    voiceHandle = voiceHandleInvalid;
  }

  if ( ImGui::Checkbox("Loop##audioLoop", &loop) )
    soloud.setLooping(voiceHandle, loop);

  ImGui::EndDisabled();

  const auto flags = ImGuiSliderFlags_NoRoundToFormat;

  if (  ImGui::DragFloat("Volume##audioVolume", &audioVolume,
                          0.01f, 0.0f, 10.0f, "%.2f", flags) )
    soloud.setVolume(voiceHandle, audioVolume);

  if (  ImGui::DragFloat("Speed##audioSpeed", &audioSpeed,
                          0.01f, std::numeric_limits <float>::epsilon(),
                         2.5f, "%.2f", flags) )
    soloud.setRelativePlaySpeed(voiceHandle, audioSpeed);
}

template <>
void
AssetManager <SoLoud::AudioSource>::ui_show(
  Json::Value& entry ) const
{
  if ( ImGui::CollapsingHeader("Type", ImGuiTreeNodeFlags_DefaultOpen) )
  {
    const auto jsonType = entry["type"];

    if ( ImGui::BeginCombo("##audioType", jsonType.asCString()) )
    {
      for ( int type = static_cast <int> (AudioSourceType::EnumBegin);
            type < static_cast <int> (AudioSourceType::EnumEnd);
            ++type )
      {
        const auto typeLabel = AudioSourceTypeToString(AudioSourceType(type));
        const bool selected = typeLabel == jsonType.asString();

        if ( ImGui::Selectable(typeLabel.c_str(), selected) )
          entry["type"] = AudioSourceTypeToString(AudioSourceType(type));
      }

      ImGui::EndCombo(); // audioType
    }
  }

  if ( ImGui::CollapsingHeader("Path", ImGuiTreeNodeFlags_DefaultOpen) )
  {
    std::string path = entry["path"].asString();
    if ( ImGui::InputText("##audioPath", &path) )
      entry["path"] = path;
  }

  if ( ImGui::CollapsingHeader("Properties", ImGuiTreeNodeFlags_DefaultOpen) )
  {
//  todo: switch ( AudioSourceType(type) )
  }
}

template class
AssetManager <SoLoud::AudioSource>;

} // namespace cqde::types

