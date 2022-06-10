#pragma once

#include <json/forwards.h>

#include <spdlog/common.h>

#include <string>


namespace cqde
{

class ConfigManager
{
  using level = spdlog::level::level_enum;

  std::string mLogPattern {"[%Y-%m-%d %T.%e] [%^%l%$] [%s:%#] %v"};

  level mLogLevelCmd {level::err};
  level mLogLevelFile {level::info};

  uint32_t mWindowWidth {800u};
  uint32_t mWindowHeight {600u};

  bool mFullscreenEnabled {false};

public:
  ConfigManager() = default;
  ConfigManager( const std::string& filename );

  virtual void write( const std::string& filename );
  virtual void setConfig( const Json::Value& config );

  virtual Json::Value read( const std::string& filename ) const;


  virtual Json::Value config() const;

  std::string logPattern() const;

  spdlog::level::level_enum logLevelCmd() const;
  spdlog::level::level_enum logLevelFile() const;

  uint32_t windowWidth() const;
  uint32_t windowHeight() const;

  bool fullscreenEnabled() const;
};

} // namespace cqde
