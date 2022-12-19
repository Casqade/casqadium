#include <cqde/conversion/soloud_string.hpp>


SoLoudBackendStringConverter::SoLoudBackendStringConverter()
{
  mBackends =
  {
    {BACKEND::AUTO, "auto"},
    {BACKEND::SDL1, "sdl1"},
    {BACKEND::SDL2, "sdl2"},
    {BACKEND::PORTAUDIO, "portaudio"},
    {BACKEND::WINMM, "winmm"},
    {BACKEND::XAUDIO2, "xaudio2"},
    {BACKEND::WASAPI, "wasapi"},
    {BACKEND::ALSA, "alsa"},
    {BACKEND::JACK, "jack"},
    {BACKEND::OSS, "oss"},
    {BACKEND::OPENAL, "openal"},
    {BACKEND::COREAUDIO, "coreaudio"},
    {BACKEND::MINIAUDIO, "miniaudio"},
    {BACKEND::NOSOUND, "nosound"},
    {BACKEND::NULLDRIVER, "null"},
  };
}

std::string
SoLoudBackendStringConverter::toString(
  const BACKEND backend ) const
{
  for ( const auto& backendPair : mBackends )
    if ( backendPair.first == backend )
      return backendPair.second;

  return mBackends.at(BACKEND::AUTO);
}

SoLoudBackendStringConverter::BACKEND
SoLoudBackendStringConverter::toBackend(
  const std::string& backend ) const
{
  for ( const auto& backendPair : mBackends )
    if ( backendPair.second == backend )
      return backendPair.first;

  return BACKEND::AUTO;
}


SoLoudResamplerStringConverter::SoLoudResamplerStringConverter()
{
  mResamplers =
  {
    {RESAMPLER::RESAMPLER_POINT, "point"},
    {RESAMPLER::RESAMPLER_LINEAR, "linear"},
    {RESAMPLER::RESAMPLER_CATMULLROM, "catmullrom"},
  };
}

std::string
SoLoudResamplerStringConverter::toString(
  const RESAMPLER resampler ) const
{
  for ( const auto& resamplerPair : mResamplers )
    if ( resamplerPair.first == resampler )
      return resamplerPair.second;

  return mResamplers.at(SOLOUD_DEFAULT_RESAMPLER);
}

SoLoudResamplerStringConverter::RESAMPLER
SoLoudResamplerStringConverter::toResampler(
  const std::string& resampler ) const
{
  for ( const auto& resamplerPair : mResamplers )
    if ( resamplerPair.second == resampler )
      return resamplerPair.first;

  return SOLOUD_DEFAULT_RESAMPLER;
}
