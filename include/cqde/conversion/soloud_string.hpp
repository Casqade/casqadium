#pragma once

#include <soloud.h>

#include <map>
#include <string>


class SoLoudBackendStringConverter
{
  using BACKEND = SoLoud::Soloud::BACKENDS;

  std::map <BACKEND, std::string> mBackends {};

public:
  SoLoudBackendStringConverter();

  std::string toString( const BACKEND ) const;
  BACKEND toBackend( const std::string& ) const;
};

class SoLoudResamplerStringConverter
{
  using RESAMPLER = SoLoud::Soloud::RESAMPLER;

  std::map <RESAMPLER, std::string> mResamplers {};

public:
  SoLoudResamplerStringConverter();

  std::string toString( const RESAMPLER ) const;
  RESAMPLER toResampler( const std::string& ) const;
};

class SoLoudAttenuationModelStringConverter
{
  using AttenuationModel = SoLoud::AudioSource::ATTENUATION_MODELS;

  std::map <AttenuationModel, std::string> mModels {};

public:
  SoLoudAttenuationModelStringConverter();

  std::string toString( const AttenuationModel ) const;
  AttenuationModel toModel( const std::string& ) const;
};
