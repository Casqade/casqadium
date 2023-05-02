#pragma once

#include <spdlog/common.h>

#include <soloud.h>

#include <json/forwards.h>

#include <string>


namespace cqde::types
{

struct Config
{
  using level = spdlog::level::level_enum;
  using level_enum = spdlog::level::level_enum;

  using BACKEND = SoLoud::Soloud::BACKENDS;
  using FLAGS = SoLoud::Soloud::FLAGS;
  using RESAMPLER = SoLoud::Soloud::RESAMPLER;

  struct
  {
    BACKEND backend {BACKEND::AUTO};
    RESAMPLER resampler {SOLOUD_DEFAULT_RESAMPLER};

    uint32_t bufferSize {};
    uint32_t sampleRate {};
    uint32_t maxActiveVoices {32};

    bool roundoffClipping {true};
    bool noFpuChange {};

  } audio {};

  struct
  {
    uint32_t tickRate {60};
    uint32_t frameRate {};

  } engine {};

  std::string lastUser {"default"};

  struct
  {
    struct
    {
      level cmd {level::err};
      level file {level::info};
      level flushOn {level::warn};

    } level {};

//    [%Y-%m-%d %T.%e] [%^%l%$] [%s:%#] [thread %t] %v
    std::string pattern {"[%T.%e] [%^%l%$] [%s:%#] [thread %t] %v"};

  } log {};

  struct
  {
    int32_t width {1600u};
    int32_t height {900u};

//    -1 for primary screen
    int32_t screen {-1};

//    wait N frames for sync
    int32_t vsync {1};

//    borderless is ignored in fullscreen mode
    bool borderless {false};
    bool fullscreen {false};

  } video {};


  Config() = default;
  virtual ~Config() = default;

  virtual Json::Value toJson() const;
  virtual void fromJson( const Json::Value& );

  virtual void reset();

  int audioFlags() const;
};

} // namespace cqde::types
