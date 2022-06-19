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
  level mLogFlushLevel {level::warn};

  uint32_t mWindowWidth {800u};
  uint32_t mWindowHeight {600u};

  bool mFullscreenEnabled {false};

  uint64_t mTickRate {60};
  uint64_t mFrameRate {0};

public:
  ConfigManager() = default;
  ConfigManager( const std::string& filename );

  static Json::CharReaderBuilder configReader();
  static Json::StreamWriterBuilder configWriter();

  virtual void write( const std::string& filename );
  virtual void setConfig( const Json::Value& config );

  virtual Json::Value read( const std::string& filename ) const;


  virtual Json::Value config() const;

  std::string logPattern() const;

  spdlog::level::level_enum logLevelCmd() const;
  spdlog::level::level_enum logLevelFile() const;
  spdlog::level::level_enum logFlushLevel() const;

  uint32_t windowWidth() const;
  uint32_t windowHeight() const;

  bool fullscreenEnabled() const;

  uint64_t tickRate() const;
  uint64_t frameRate() const;
};

} // namespace cqde
