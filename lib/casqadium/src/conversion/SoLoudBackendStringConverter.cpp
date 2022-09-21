#include <cqde/conversion/SoLoudBackendStringConverter.hpp>


SoLoudBackendStringConverter::SoLoudBackendStringConverter()
{
  mBackends =
  {
    {SoLoud::Soloud::BACKENDS::AUTO, "auto"},
    {SoLoud::Soloud::BACKENDS::SDL1, "sdl1"},
    {SoLoud::Soloud::BACKENDS::SDL2, "sdl2"},
    {SoLoud::Soloud::BACKENDS::PORTAUDIO, "portaudio"},
    {SoLoud::Soloud::BACKENDS::WINMM, "winmm"},
    {SoLoud::Soloud::BACKENDS::XAUDIO2, "xaudio2"},
    {SoLoud::Soloud::BACKENDS::WASAPI, "wasapi"},
    {SoLoud::Soloud::BACKENDS::ALSA, "alsa"},
    {SoLoud::Soloud::BACKENDS::JACK, "jack"},
    {SoLoud::Soloud::BACKENDS::OSS, "oss"},
    {SoLoud::Soloud::BACKENDS::OPENAL, "openal"},
    {SoLoud::Soloud::BACKENDS::COREAUDIO, "coreaudio"},
    {SoLoud::Soloud::BACKENDS::MINIAUDIO, "miniaudio"},
    {SoLoud::Soloud::BACKENDS::NOSOUND, "nosound"},
    {SoLoud::Soloud::BACKENDS::NULLDRIVER, "null"},
  };
}

std::string
SoLoudBackendStringConverter::toString(
  const BACKEND backend ) const
{
  using BACKEND = SoLoud::Soloud::BACKENDS;

  for ( const auto& backendPair : mBackends )
    if ( backendPair.first == backend )
      return backendPair.second;

  return mBackends.at(BACKEND::AUTO);
}

SoLoudBackendStringConverter::BACKEND
SoLoudBackendStringConverter::toBackend(
  const std::string& backend ) const
{
  using BACKEND = SoLoud::Soloud::BACKENDS;

  for ( const auto& backendPair : mBackends )
    if ( backendPair.second == backend )
      return backendPair.first;

  return BACKEND::AUTO;
}
