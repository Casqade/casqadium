#include <cqde/types/ConfigManager.hpp>
#include <cqde/common.hpp>

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

  return root;
}();

ConfigManager::ConfigManager(
  const std::filesystem::path& path )
  : ConfigManager()
{
  setConfig(read(path));
  write(path);
}

Json::Value
ConfigManager::config() const
{
  Json::Value config {};

  config["log"]["pattern"] = mLogPattern;

  config["log"]["level"]["cmd"] = to_string_view(mLogLevelCmd).data();
  config["log"]["level"]["file"] = to_string_view(mLogLevelFile).data();
  config["log"]["level"]["flush-on"] = to_string_view(mLogFlushLevel).data();

  config["video"]["window-width"] = mWindowWidth;
  config["video"]["window-height"] = mWindowHeight;

  config["video"]["fullscreen"] = mFullscreenEnabled;

  config["engine"]["tick-rate"] = mTickRate;
  config["engine"]["frame-rate"] = mFrameRate;

  return config;
}

void
ConfigManager::setConfig( const Json::Value& config )
{
  mLogPattern = config["log"]["pattern"].asString();

  mLogLevelCmd = spdlog::level::from_str(config["log"]["level"]["cmd"].asString());
  mLogLevelFile = spdlog::level::from_str(config["log"]["level"]["file"].asString());
  mLogFlushLevel = spdlog::level::from_str(config["log"]["level"]["flush-on"].asString());

  mWindowWidth = config["video"]["window-width"].asUInt();
  mWindowHeight = config["video"]["window-height"].asUInt();
  mFullscreenEnabled = config["video"]["fullscreen"].asBool();

  mTickRate = config["engine"]["tick-rate"].asUInt64();
  mFrameRate = config["engine"]["frame-rate"].asUInt64();
}

Json::Value
ConfigManager::read( const std::filesystem::path& path ) const
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
    std::cerr << format("Error: Can't read configuration file ({})\n", e.what());
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


  if ( configIn["video"]["window-width"].isUInt() && configIn["video"]["window-width"].asUInt() > 0 )
     result["video"]["window-width"] = configIn["video"]["window-width"].asUInt();
  else
    result["video"]["window-width"] = configDefault["video"]["window-width"].asUInt();

  if ( configIn["video"]["window-height"].isUInt() && configIn["video"]["window-height"].asUInt() > 0 )
     result["video"]["window-height"] = configIn["video"]["window-height"].asUInt();
  else
    result["video"]["window-height"] = configDefault["video"]["window-height"].asUInt();


  if ( configIn["engine"]["tick-rate"].isUInt() )
    result["engine"]["tick-rate"] = configIn["engine"]["tick-rate"];
  else
    result["engine"]["tick-rate"] = configDefault["engine"]["tick-rate"];

  if ( configIn["engine"]["frame-rate"].isUInt() )
    result["engine"]["frame-rate"] = configIn["engine"]["frame-rate"];
  else
    result["engine"]["frame-rate"] = configDefault["engine"]["frame-rate"];

  return result;
}

void
ConfigManager::write( const std::filesystem::path& path )
{
  try
  {
    auto configOut = fileOpen( path, std::ios::out | std::ios::trunc );
    configOut << Json::writeString(jsonWriter(), config());
  }
  catch ( const std::exception& e )
  {
    std::cerr << format("Error: Can't write configuration file ({})\n", e.what());
  }
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

} // namespace cqde
