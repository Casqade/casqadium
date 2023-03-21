#pragma once

#include <cqde/alias.hpp>

#include <soloud.h>

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>


namespace cqde::types
{

class AudioFilterFactory
{
  using AudioFilter = SoLoud::Filter;
  using AudioFilterInstance = SoLoud::FilterInstance;

  std::unordered_map <std::string, std::shared_ptr <AudioFilter>> mFilters {};

public:
  AudioFilterFactory() = default;

  template <class T>
  void registerFilter( const std::string& id );

  AudioFilter* get( const std::string& id ) const;
  AudioFilterInstance* create( const std::string& id ) const;

  std::vector <std::string> filters() const;
};

template <class T>
void
AudioFilterFactory::registerFilter(
  const std::string& id )
{
  mFilters[id] = std::make_shared <T> ();
}

} // namespace cqde::types
