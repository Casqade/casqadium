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
  using RESAMPLER = SoLoud::Soloud::RESAMPLER;

  std::string mExecutableName {};
  bool mEditorMode {};

//  [%Y-%m-%d %T.%e] [%^%l%$] [%s:%#] [thread %t] %v
  std::string mLogPattern {"[%T.%e] [%^%l%$] [%s:%#] [thread %t] %v"};

  level mLogLevelCmd {level::err};
  level mLogLevelFile {level::info};
  level mLogFlushLevel {level::warn};

  uint32_t mWindowWidth {1600u};
  uint32_t mWindowHeight {900u};

  bool mFullscreenEnabled {false};

  uint64_t mTickRate {60};
  uint64_t mFrameRate {60};

  BACKEND mAudioBackend {BACKEND::AUTO};
  RESAMPLER mAudioResampler {SOLOUD_DEFAULT_RESAMPLER};

  uint32_t mAudioBufferSize {};
  uint32_t mAudioSampleRate {};
  uint32_t mAudioMaxActiveVoices {32};

  bool mAudioRoudoffClipping {true};
  bool mAudioNoFpuChange {};

  UserId mLastUser {"default"};

public:
  ConfigManager() = default;
  ConfigManager( const std::string& executableName, const bool editorMode = false, path configPath = {} );
  virtual ~ConfigManager() = default;

  virtual void write( const path& );
  virtual void setConfig( const Json::Value& config );

  virtual Json::Value read( const path& ) const;

  virtual Json::Value config() const;

  std::string executableName() const;
  bool editorMode() const;

  std::string logPattern() const;

  level_enum logLevelCmd() const;
  level_enum logLevelFile() const;
  level_enum logFlushLevel() const;

  uint32_t windowWidth() const;
  uint32_t windowHeight() const;

  bool fullscreenEnabled() const;

  uint64_t tickRate() const;
  uint64_t frameRate() const;

  int audioFlags() const;
  BACKEND audioBackend() const;
  RESAMPLER audioResampler() const;

  uint32_t audioBufferSize() const;
  uint32_t audioSampleRate() const;
  uint32_t audioMaxActiveVoices() const;

  bool audioRoundoffClipping() const;
  bool audioNoFpuChange() const;

  UserId lastUser() const;
};

} // namespace cqde
