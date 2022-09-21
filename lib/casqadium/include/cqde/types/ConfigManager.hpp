#pragma once

#include <cqde/alias.hpp>

#include <soloud.h>

#include <spdlog/common.h>

#include <json/forwards.h>

#include <string>
#include <filesystem>


namespace cqde::types
{

class ConfigManager
{
  using path = std::filesystem::path;
  using level = spdlog::level::level_enum;
  using level_enum = spdlog::level::level_enum;

  using BACKEND = SoLoud::Soloud::BACKENDS;
  using FLAGS = SoLoud::Soloud::FLAGS;

//  [%Y-%m-%d %T.%e] [%^%l%$] [%s:%#] [thread %t] %v
  std::string mLogPattern {"[%T.%e] [%^%l%$] [%s:%#] [thread %t] %v"};

  level mLogLevelCmd {level::err};
  level mLogLevelFile {level::info};
  level mLogFlushLevel {level::warn};

  uint32_t mWindowWidth {800u};
  uint32_t mWindowHeight {600u};

  bool mFullscreenEnabled {false};

  uint64_t mTickRate {60};
  uint64_t mFrameRate {0};

  BACKEND mAudioBackend {};

  uint32_t mAudioBufferSize {};
  uint32_t mAudioSampleRate {};

  bool mAudioRoudoffClipping {true};
  bool mAudioNoFpuChange {};

  UserId mLastUser {"default"};

public:
  ConfigManager() = default;
  ConfigManager( const path& configPath );

  virtual void write( const path& );
  virtual void setConfig( const Json::Value& config );

  virtual Json::Value read( const path& ) const;

  virtual Json::Value config() const;

  std::string logPattern() const;

  level_enum logLevelCmd() const;
  level_enum logLevelFile() const;
  level_enum logFlushLevel() const;

  uint32_t windowWidth() const;
  uint32_t windowHeight() const;

  bool fullscreenEnabled() const;

  uint64_t tickRate() const;
  uint64_t frameRate() const;

  BACKEND audioBackend() const;
  int audioFlags() const;

  uint32_t audioBufferSize() const;
  uint32_t audioSampleRate() const;

  bool audioRoundoffClipping() const;
  bool audioNoFpuChange() const;

  UserId lastUser() const;
};

} // namespace cqde
