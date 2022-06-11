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

  try
  {
    config["log"]["pattern"] = configSrc["log"]["pattern"].asString();
    config["log"]["level"]["cmd"] = configSrc["log"]["level"]["cmd"].asString();
    config["log"]["level"]["file"] = configSrc["log"]["level"]["file"].asString();

    const uint32_t windowW = configSrc["video"]["window-width"].asUInt();
    const uint32_t windowH = configSrc["video"]["window-height"].asUInt();

    if ( windowW == 0 || windowH == 0 )
      throw Json::LogicError("Window dimensions can't be zero");

    config["video"]["window-width"] = windowW;
    config["video"]["window-height"] = windowH;

    config["engine"]["tick-rate"] = configSrc["tick-rate"];
    config["engine"]["frame-rate"] = configSrc["frame-rate"];
  }
  catch ( Json::LogicError& e )
  {
    std::cerr << "Error: invalid values in config " << filename << ": " << e.what() << "\n";
    config = ConfigManager().config();
  }

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
