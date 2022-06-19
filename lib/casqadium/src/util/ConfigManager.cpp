#include <cqde/util/ConfigManager.hpp>

#include <json/value.h>
#include <json/reader.h>
#include <json/writer.h>

#include <iostream>
#include <fstream>


namespace cqde
{

ConfigManager::ConfigManager(
  const std::string& filename )
  : ConfigManager()
{
  setConfig( read(filename) );
  write(filename);
}

Json::Value
ConfigManager::config() const
{
  Json::Value config {};

  config["log"]["pattern"] = mLogPattern;

  config["log"]["level"]["cmd"] = to_string_view(mLogLevelCmd).data();
  config["log"]["level"]["file"] = to_string_view(mLogLevelFile).data();

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

  mWindowWidth = config["video"]["window-width"].asUInt();
  mWindowHeight = config["video"]["window-height"].asUInt();
  mFullscreenEnabled = config["video"]["fullscreen"].asBool();

  mTickRate = config["engine"]["tick-rate"].asUInt64();
  mFrameRate = config["engine"]["frame-rate"].asUInt64();
}

Json::Value
ConfigManager::read( const std::string& filename ) const
{
  Json::Value config {};

  std::ifstream configIn( filename, std::ios::in );
  if ( configIn.is_open() == false )
  {
    std::cerr << "Error: failed to read config from '" << filename << "'\n";

    return ConfigManager().config();
  }

  Json::Value configSrc {};
  Json::String parseErrors {};

  if ( Json::parseFromStream( configReader(), configIn,
                              &configSrc, &parseErrors ) == false )
  {
    configSrc = ConfigManager().config();
    std::cerr << "Error: failed to parse config " << filename << ": "
              << parseErrors;
  }
  configIn.close();

  const auto configDefault = ConfigManager().config();

  if ( configSrc["log"]["pattern"].isString() )
    config["log"]["pattern"] = configSrc["log"]["pattern"].asString();
  else
    config["log"]["pattern"] = configDefault["log"]["pattern"].asString();

  if ( configSrc["log"]["level"]["cmd"].isString() )
    config["log"]["level"]["cmd"] = configSrc["log"]["level"]["cmd"].asString();
  else
    config["log"]["level"]["cmd"] = configDefault["log"]["level"]["cmd"].asString();

  if ( configSrc["log"]["level"]["file"].isString() )
    config["log"]["level"]["file"] = configSrc["log"]["level"]["file"].asString();
  else
    config["log"]["level"]["file"] = configDefault["log"]["level"]["file"].asString();



  if ( configSrc["video"]["window-width"].isUInt() && configSrc["video"]["window-width"].asUInt() > 0 )
     config["video"]["window-width"] = configSrc["video"]["window-width"].asUInt();
  else
    config["video"]["window-width"] = configDefault["video"]["window-width"].asUInt();

  if ( configSrc["video"]["window-height"].isUInt() && configSrc["video"]["window-height"].asUInt() > 0 )
     config["video"]["window-height"] = configSrc["video"]["window-height"].asUInt();
  else
    config["video"]["window-height"] = configDefault["video"]["window-height"].asUInt();


  if ( configSrc["engine"]["tick-rate"].isUInt() )
    config["engine"]["tick-rate"] = configSrc["engine"]["tick-rate"];
  else
    config["engine"]["tick-rate"] = configDefault["engine"]["tick-rate"];

  if ( configSrc["engine"]["frame-rate"].isUInt() )
    config["engine"]["frame-rate"] = configSrc["engine"]["frame-rate"];
  else
    config["engine"]["frame-rate"] = configDefault["engine"]["frame-rate"];

  return config;
}

void
ConfigManager::write( const std::string& filename )
{
  using namespace spdlog::level;

  std::ofstream configOut( filename, std::ios::out | std::ios::trunc );
  if ( configOut.is_open() == false )
  {
    std::cerr << "Error: failed to write config to '" << filename << "'\n";
    return;
  }

  std::unique_ptr <Json::StreamWriter> writer(configWriter().newStreamWriter());
  writer->write( config(), &configOut );
  configOut.close();
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

Json::CharReaderBuilder
ConfigManager::configReader()
{
  Json::CharReaderBuilder reader {};

  reader["collectComments"] = true;
  reader["allowComments"] = true;
  reader["allowTrailingCommas"] = true;
  reader["strictRoot"] = false;
  reader["allowDroppedNullPlaceholders"] = false;
  reader["allowNumericKeys"] = false;
  reader["allowSingleQuotes"] = false;
  reader["stackLimit"] = 1000;
  reader["failIfExtra"] = false;
  reader["rejectDupKeys"] = true;
  reader["allowSpecialFloats"] = true;
  reader["skipBom"] = true;

  return reader;
}

Json::StreamWriterBuilder
ConfigManager::configWriter()
{
  Json::StreamWriterBuilder writer {};

  writer["indentation"] = "  ";
  writer["commentStyle"] = "All";
  writer["precisionType"] = "  ";
  writer["enableYAMLCompatibility"] = false;
  writer["dropNullPlaceholders"] = false;
  writer["useSpecialFloats"] = true;
  writer["emitUTF8"] = true;
  writer["precision"] = 17;
  writer["precisionType"] = "significant";

  return writer;
}

} // namespace cqde
