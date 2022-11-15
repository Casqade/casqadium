#include <cqde/types/ConfigManager.hpp>

#include <cqde/common.hpp>
#include <cqde/file_helpers.hpp>
#include <cqde/json_helpers.hpp>

#include <cqde/conversion/soloud_string.hpp>

#include <spdlog/fmt/bundled/format.h>

#include <json/value.h>
#include <json/reader.h>
#include <json/writer.h>

#include <iostream>
#include <fstream>


namespace cqde::types
{


const static Json::Value configReference =
[]
{
  using ValueType = Json::ValueType;
  using namespace std::string_literals;

  Json::Value root = ValueType::objectValue;
  root.setComment("// config root must be a JSON object"s,
                  Json::CommentPlacement::commentBefore);

  Json::Value& engine = root["engine"];
  engine = ValueType::objectValue;
  engine.setComment("// 'engine' value must be a JSON object"s,
                    Json::CommentPlacement::commentBefore);

  Json::Value& framerate = engine["frame-rate"];
  framerate = ValueType::uintValue;
  framerate.setComment("// 'frame-rate' value must be a JSON unsigned integer"s,
                        Json::CommentPlacement::commentBefore);

  Json::Value& tickrate = engine["tick-rate"];
  tickrate = ValueType::uintValue;
  tickrate.setComment("// 'tick-rate' value must be a JSON unsigned integer"s,
                      Json::CommentPlacement::commentBefore);


  Json::Value& logConfig = root["log"];
  logConfig = ValueType::objectValue;
  logConfig.setComment("// 'log' value must be a JSON object"s,
                       Json::CommentPlacement::commentBefore);

  Json::Value& pattern = logConfig["pattern"];
  pattern = ValueType::stringValue;
  pattern.setComment("// 'pattern' value must be a JSON string"s,
                      Json::CommentPlacement::commentBefore);


  Json::Value& level= logConfig["level"];
  level = ValueType::objectValue;
  level.setComment("// 'level' value must be a JSON object"s,
                    Json::CommentPlacement::commentBefore);

  Json::Value& cmd = level["cmd"];
  cmd = ValueType::stringValue;
  cmd.setComment("// 'cmd' value must be a JSON string"s,
                  Json::CommentPlacement::commentBefore);

  Json::Value& file = level["file"];
  file = ValueType::stringValue;
  file.setComment("// 'file' value must be a JSON string"s,
                  Json::CommentPlacement::commentBefore);

  Json::Value& flushOn = level["flush-on"];
  flushOn = ValueType::stringValue;
  flushOn.setComment("// 'flush-on' value must be a JSON string"s,
                      Json::CommentPlacement::commentBefore);


  Json::Value& video = root["video"];
  video = ValueType::objectValue;
  video.setComment("// 'video' value must be a JSON object"s,
                    Json::CommentPlacement::commentBefore);

  Json::Value& fullscreen = video["fullscreen"];
  fullscreen = ValueType::booleanValue;
  fullscreen.setComment("// 'fullscreen' value must be a JSON boolean"s,
                        Json::CommentPlacement::commentBefore);

  Json::Value& windowW = video["window-width"];
  windowW = ValueType::uintValue;
  windowW.setComment("// 'window-width' value must be a JSON unsigned integer"s,
                      Json::CommentPlacement::commentBefore);

  Json::Value& windowH = video["window-height"];
  windowH = ValueType::uintValue;
  windowH.setComment("// 'window-height' value must be a JSON unsigned integer"s,
                      Json::CommentPlacement::commentBefore);


  Json::Value& audio = root["audio"];
  audio = ValueType::objectValue;
  audio.setComment("// 'audio' value must be a JSON object"s,
                    Json::CommentPlacement::commentBefore);

  Json::Value& backend = audio["backend"];
  backend = ValueType::stringValue;
  backend.setComment("// 'backend' value must be a JSON string"s,
                      Json::CommentPlacement::commentBefore);

  Json::Value& resampler = audio["resampler"];
  resampler = ValueType::stringValue;
  resampler.setComment("// 'resampler' value must be a JSON string"s,
                        Json::CommentPlacement::commentBefore);

  Json::Value& bufferSize = audio["buffer-size"];
  bufferSize = ValueType::stringValue;
  bufferSize.setComment("// 'buffer-size' value must be a JSON unsigned integer"s,
                        Json::CommentPlacement::commentBefore);

  Json::Value& sampleRate = audio["sample-rate"];
  sampleRate = ValueType::stringValue;
  sampleRate.setComment("// 'sample-rate' value must be a JSON unsigned integer"s,
                        Json::CommentPlacement::commentBefore);

  Json::Value& maxActiveVoices = audio["max-active-voices"];
  maxActiveVoices = ValueType::stringValue;
  maxActiveVoices.setComment("// 'maxActiveVoices' value must be a JSON unsigned integer"s,
                              Json::CommentPlacement::commentBefore);

  Json::Value& audioFlags = audio["flags"];
  audioFlags = ValueType::objectValue;
  audioFlags.setComment("// 'flags' value must be a JSON object"s,
                        Json::CommentPlacement::commentBefore);

  Json::Value& roundoff = audioFlags["roundoff-clipping"];
  roundoff = ValueType::booleanValue;
  roundoff.setComment("// 'roundoff-clipping' value must be a JSON boolean"s,
                      Json::CommentPlacement::commentBefore);

  Json::Value& noFpuChange = audioFlags["no-fpu-change"];
  noFpuChange = ValueType::booleanValue;
  noFpuChange.setComment("// 'no-fpu-change' value must be a JSON boolean"s,
                          Json::CommentPlacement::commentBefore);


  Json::Value& lastUser = root["last-user"];
  lastUser = ValueType::stringValue;
  lastUser.setComment("// 'last-user' value must be a JSON string"s,
                      Json::CommentPlacement::commentBefore);

  return root;
}();

ConfigManager::ConfigManager(
  const std::string& executableName,
  path configPath )
  : mExecutableName{executableName}
{
  if ( configPath.empty() == true )
    configPath = executableName + ".conf";

  ConfigManager::setConfig(ConfigManager::read(configPath));
  ConfigManager::write(configPath);
}

Json::Value
ConfigManager::config() const
{
  Json::Value config {};

  config["log"]["pattern"] = mLogPattern;

  config["log"]["level"]["cmd"] = to_string_view(mLogLevelCmd).data();
  config["log"]["level"]["file"] = to_string_view(mLogLevelFile).data();
  config["log"]["level"]["flush-on"] = to_string_view(mLogFlushLevel).data();

  config["engine"]["tick-rate"] = mTickRate;
  config["engine"]["frame-rate"] = mFrameRate;

  config["video"]["window-width"] = mWindowWidth;
  config["video"]["window-height"] = mWindowHeight;

  config["video"]["fullscreen"] = mFullscreenEnabled;

  config["audio"]["backend"] = SoLoudBackendStringConverter{}.toString(mAudioBackend);
  config["audio"]["resampler"] = SoLoudResamplerStringConverter{}.toString(mAudioResampler);

  config["audio"]["buffer-size"] = mAudioBufferSize;
  config["audio"]["sample-rate"] = mAudioSampleRate;
  config["audio"]["max-active-voices"] = mAudioMaxActiveVoices;

  config["audio"]["flags"]["roundoff-clipping"] = mAudioRoudoffClipping;
  config["audio"]["flags"]["no-fpu-change"] = mAudioNoFpuChange;

  config["last-user"] = mLastUser.str();

  return config;
}

void
ConfigManager::setConfig(
  const Json::Value& config )
{
  mLogPattern = config["log"]["pattern"].asString();

  mLogLevelCmd = spdlog::level::from_str(config["log"]["level"]["cmd"].asString());
  mLogLevelFile = spdlog::level::from_str(config["log"]["level"]["file"].asString());
  mLogFlushLevel = spdlog::level::from_str(config["log"]["level"]["flush-on"].asString());

  mTickRate = config["engine"]["tick-rate"].asUInt64();
  mFrameRate = config["engine"]["frame-rate"].asUInt64();

  mWindowWidth = config["video"]["window-width"].asUInt();
  mWindowHeight = config["video"]["window-height"].asUInt();

  mFullscreenEnabled = config["video"]["fullscreen"].asBool();

  mAudioBackend = SoLoudBackendStringConverter{}.toBackend(config["audio"]["backend"].asString());
  mAudioResampler = SoLoudResamplerStringConverter{}.toResampler(config["audio"]["resampler"].asString());

  mAudioBufferSize = config["audio"]["buffer-size"].asUInt();
  mAudioSampleRate = config["audio"]["sample-rate"].asUInt();
  mAudioMaxActiveVoices = config["audio"]["max-active-voices"].asUInt();

  mAudioRoudoffClipping = config["audio"]["flags"]["roundoff-clipping"].asBool();
  mAudioNoFpuChange = config["audio"]["flags"]["no-fpu-change"].asBool();
}

Json::Value
ConfigManager::read(
  const path& path ) const
{
  Json::Value result {};
  Json::Value configIn {};

  const Json::Value configDefault = ConfigManager().config();

  try
  {
    configIn = fileParse(path);
  }
  catch ( const std::exception& e )
  {
    configIn = configDefault;

    using fmt::format;
    std::cerr <<
      format("Error: Can't read configuration file ({})\n",
             e.what());
  }

  if ( configIn["log"]["pattern"].isString() )
    result["log"]["pattern"] = configIn["log"]["pattern"].asString();
  else
    result["log"]["pattern"] = configDefault["log"]["pattern"].asString();

  if ( configIn["log"]["level"]["cmd"].isString() )
    result["log"]["level"]["cmd"] = configIn["log"]["level"]["cmd"].asString();
  else
    result["log"]["level"]["cmd"] = configDefault["log"]["level"]["cmd"].asString();

  if ( configIn["log"]["level"]["file"].isString() )
    result["log"]["level"]["file"] = configIn["log"]["level"]["file"].asString();
  else
    result["log"]["level"]["file"] = configDefault["log"]["level"]["file"].asString();

  if ( configIn["log"]["level"]["flush-on"].isString() )
    result["log"]["level"]["flush-on"] = configIn["log"]["level"]["flush-on"].asString();
  else
    result["log"]["level"]["flush-on"] = configDefault["log"]["level"]["flush-on"].asString();


  if ( configIn["engine"]["tick-rate"].isUInt() )
    result["engine"]["tick-rate"] = configIn["engine"]["tick-rate"];
  else
    result["engine"]["tick-rate"] = configDefault["engine"]["tick-rate"];

  if ( configIn["engine"]["frame-rate"].isUInt() )
    result["engine"]["frame-rate"] = configIn["engine"]["frame-rate"];
  else
    result["engine"]["frame-rate"] = configDefault["engine"]["frame-rate"];


  if ( configIn["video"]["window-width"].isUInt() && configIn["video"]["window-width"].asUInt() > 0 )
     result["video"]["window-width"] = configIn["video"]["window-width"].asUInt();
  else
    result["video"]["window-width"] = configDefault["video"]["window-width"].asUInt();

  if ( configIn["video"]["window-height"].isUInt() && configIn["video"]["window-height"].asUInt() > 0 )
     result["video"]["window-height"] = configIn["video"]["window-height"].asUInt();
  else
    result["video"]["window-height"] = configDefault["video"]["window-height"].asUInt();


  if ( configIn["audio"]["backend"].isString() )
    result["audio"]["backend"] = configIn["audio"]["backend"];
  else
    result["audio"]["backend"] = configDefault["audio"]["backend"];

  if ( configIn["audio"]["resampler"].isString() )
    result["audio"]["resampler"] = configIn["audio"]["resampler"];
  else
    result["audio"]["resampler"] = configDefault["audio"]["resampler"];

  if ( configIn["audio"]["buffer-size"].isUInt() )
    result["audio"]["buffer-size"] = configIn["audio"]["buffer-size"];
  else
    result["audio"]["buffer-size"] = configDefault["audio"]["buffer-size"];

  if ( configIn["audio"]["sample-rate"].isUInt() )
    result["audio"]["sample-rate"] = configIn["audio"]["sample-rate"];
  else
    result["audio"]["sample-rate"] = configDefault["audio"]["sample-rate"];

  if ( configIn["audio"]["max-active-voices"].isUInt() )
    result["audio"]["max-active-voices"] = configIn["audio"]["max-active-voices"];
  else
    result["audio"]["max-active-voices"] = configDefault["audio"]["max-active-voices"];

  if ( configIn["audio"]["flags"]["roundoff-clipping"].isBool() )
    result["audio"]["flags"]["roundoff-clipping"] = configIn["audio"]["flags"]["roundoff-clipping"];
  else
    result["audio"]["flags"]["roundoff-clipping"] = configDefault["audio"]["flags"]["roundoff-clipping"];

  if ( configIn["audio"]["flags"]["no-fpu-change"].isBool() )
    result["audio"]["flags"]["no-fpu-change"] = configIn["audio"]["flags"]["no-fpu-change"];
  else
    result["audio"]["flags"]["no-fpu-change"] = configDefault["audio"]["flags"]["no-fpu-change"];


  if ( configIn["last-user"].isString() )
    result["last-user"] = configIn["last-user"];
  else
    result["last-user"] = configDefault["last-user"].asString();

  return result;
}

void
ConfigManager::write(
  const path& path )
{
  try
  {
    const auto streamFlags = std::ios::out |
                             std::ios::trunc |
                             std::ios::binary;

    auto fileStream = fileOpen(path, streamFlags);
    fileStream << Json::writeString(jsonWriter(), config());
  }
  catch ( const std::exception& e )
  {
    using fmt::format;
    std::cerr <<
      format("Error: Can't write configuration file ({})\n",
             e.what());
  }
}

std::string
ConfigManager::executableName() const
{
  return mExecutableName;
}

std::string
ConfigManager::logPattern() const
{
  return mLogPattern;
}

spdlog::level::level_enum
ConfigManager::logLevelCmd() const
{
  return mLogLevelCmd;
}

spdlog::level::level_enum
ConfigManager::logLevelFile() const
{
  return mLogLevelFile;
}

spdlog::level::level_enum
ConfigManager::logFlushLevel() const
{
  return mLogFlushLevel;
}

uint32_t
ConfigManager::windowWidth() const
{
  return mWindowWidth;
}

uint32_t
ConfigManager::windowHeight() const
{
  return mWindowHeight;
}

bool
ConfigManager::fullscreenEnabled() const
{
  return mFullscreenEnabled;
}

uint64_t
ConfigManager::tickRate() const
{
  return mTickRate;
}

uint64_t
ConfigManager::frameRate() const
{
  return mFrameRate;
}

int
ConfigManager::audioFlags() const
{
  int flags {};

  if ( mAudioRoudoffClipping == true )
    flags |= FLAGS::CLIP_ROUNDOFF;

  if ( mAudioNoFpuChange == true )
    flags |= FLAGS::NO_FPU_REGISTER_CHANGE;

  return flags;
}

ConfigManager::BACKEND
ConfigManager::audioBackend() const
{
  return mAudioBackend;
}

ConfigManager::RESAMPLER
ConfigManager::audioResampler() const
{
  return mAudioResampler;
}

uint32_t
ConfigManager::audioBufferSize() const
{
  return mAudioBufferSize;
}

uint32_t
ConfigManager::audioSampleRate() const
{
  return mAudioSampleRate;
}

uint32_t
ConfigManager::audioMaxActiveVoices() const
{
  return mAudioMaxActiveVoices;
}

bool
ConfigManager::audioRoundoffClipping() const
{
  return mAudioRoudoffClipping;
}

bool
ConfigManager::audioNoFpuChange() const
{
  return mAudioNoFpuChange;
}

UserId
ConfigManager::lastUser() const
{
  return mLastUser;
}

} // namespace cqde
