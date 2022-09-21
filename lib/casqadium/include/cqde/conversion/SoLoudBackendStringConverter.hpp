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
