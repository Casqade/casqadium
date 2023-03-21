#include <cqde/types/Config.hpp>

#include <cqde/conversion/soloud_string.hpp>

#include <json/value.h>


namespace cqde::types
{

Json::Value
Config::toJson() const
{
  Json::Value config {};

  config["log"]["pattern"] = log.pattern;

  config["log"]["level"]["cmd"] = to_string_view(log.level.cmd).data();
  config["log"]["level"]["file"] = to_string_view(log.level.file).data();
  config["log"]["level"]["flush-on"] = to_string_view(log.level.flushOn).data();

  config["engine"]["tick-rate"] = engine.tickRate;
  config["engine"]["frame-rate"] = engine.frameRate;

  config["video"]["width"] = video.width;
  config["video"]["height"] = video.height;
  config["video"]["screen"] = video.screen;
  config["video"]["vsync"] = video.vsync;
  config["video"]["borderless"] = video.borderless;
  config["video"]["fullscreen"] = video.fullscreen;

  config["audio"]["backend"] = SoLoudBackendStringConverter{}.toString(audio.backend);
  config["audio"]["resampler"] = SoLoudResamplerStringConverter{}.toString(audio.resampler);

  config["audio"]["buffer-size"] = audio.bufferSize;
  config["audio"]["sample-rate"] = audio.sampleRate;
  config["audio"]["max-active-voices"] = audio.maxActiveVoices;

  config["audio"]["flags"]["roundoff-clipping"] = audio.roundoffClipping;
  config["audio"]["flags"]["no-fpu-change"] = audio.noFpuChange;

  config["last-user"] = lastUser;

  return config;
}

void
Config::fromJson(
  const Json::Value& json )
{
  reset();

  if ( json["log"]["pattern"].isString() )
    log.pattern = json["log"]["pattern"].asString();

  if ( json["log"]["level"]["cmd"].isString() )
    log.level.cmd = spdlog::level::from_str(json["log"]["level"]["cmd"].asString());

  if ( json["log"]["level"]["file"].isString() )
    log.level.file = spdlog::level::from_str(json["log"]["level"]["file"].asString());

  if ( json["log"]["level"]["flush-on"].isString() )
    log.level.flushOn = spdlog::level::from_str(json["log"]["level"]["flush-on"].asString());


  if ( json["engine"]["tick-rate"].isUInt() )
    engine.tickRate = json["engine"]["tick-rate"].asUInt();

  if ( json["engine"]["frame-rate"].isUInt() )
    engine.frameRate = json["engine"]["frame-rate"].asUInt();


  if ( json["video"]["width"].isInt() )
     video.width = json["video"]["width"].asInt();

  if ( json["video"]["height"].isInt() )
     video.height = json["video"]["height"].asInt();

  if ( json["video"]["screen"].isInt() )
    video.screen = json["video"]["screen"].asInt();

  if ( json["video"]["vsync"].isInt() )
    video.vsync = json["video"]["vsync"].asInt();

  if ( json["video"]["borderless"].isBool() )
    video.borderless = json["video"]["borderless"].asBool();

  if ( json["video"]["fullscreen"].isBool() )
    video.fullscreen = json["video"]["fullscreen"].asBool();


  if ( json["audio"]["backend"].isString() )
    audio.backend = SoLoudBackendStringConverter{}.toBackend(json["audio"]["backend"].asString());

  if ( json["audio"]["resampler"].isString() )
    audio.resampler = SoLoudResamplerStringConverter{}.toResampler(json["audio"]["resampler"].asString());

  if ( json["audio"]["buffer-size"].isUInt() )
    audio.bufferSize = json["audio"]["buffer-size"].asUInt();

  if ( json["audio"]["sample-rate"].isUInt() )
    audio.sampleRate = json["audio"]["sample-rate"].asUInt();

  if ( json["audio"]["max-active-voices"].isUInt() )
    audio.maxActiveVoices = json["audio"]["max-active-voices"].asUInt();

  if ( json["audio"]["flags"]["roundoff-clipping"].isBool() )
    audio.roundoffClipping = json["audio"]["flags"]["roundoff-clipping"].asBool();

  if ( json["audio"]["flags"]["no-fpu-change"].isBool() )
    audio.noFpuChange = json["audio"]["flags"]["no-fpu-change"].asBool();


  if ( json["last-user"].isString() )
    lastUser = json["last-user"].asString();
}

void
Config::reset()
{
  *this = Config{};
}

int
Config::audioFlags() const
{
  using FLAGS = SoLoud::Soloud::FLAGS;

  int flags {};

  if ( audio.roundoffClipping == true )
    flags |= FLAGS::CLIP_ROUNDOFF;

  if ( audio.noFpuChange == true )
    flags |= FLAGS::NO_FPU_REGISTER_CHANGE;

  return flags;
}

} // namespace cqde::types
