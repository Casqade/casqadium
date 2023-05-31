#include <cqde/types/audio/AudioFilterFactory.hpp>


namespace cqde::types
{

AudioFilterFactory::AudioFilter*
AudioFilterFactory::get(
  const std::string& id ) const
{
  if ( mFilters.count(id) > 0 )
    return mFilters.at(id).get();

  return {};
}

AudioFilterFactory::AudioFilterInstance*
AudioFilterFactory::create(
  const std::string& id ) const
{
  const auto filter = get(id);

  if ( filter != nullptr )
    return filter->createInstance();

  return nullptr;
}

std::vector <std::string>
AudioFilterFactory::filters() const
{
  std::vector <std::string> filters {};

  for ( const auto& [id, filter] : mFilters )
    filters.push_back(id);

  return filters;
}

} // namespace cqde::types
