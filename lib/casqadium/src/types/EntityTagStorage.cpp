#include <cqde/types/EntityTagStorage.hpp>

#include <entt/entity/entity.hpp>

#include <string>


namespace cqde::types
{

EntityTagStorage::EntityTagStorage()
  : tags({{entt::hashed_string("null"), entt::null}})
{}

entt::entity
EntityTagStorage::get( const entt::hashed_string& id ) const
{
  try
  {
    return tags.at(id);
  }
  catch (...)
  {
    return entt::null;
  }
}

entt::hashed_string
EntityTagStorage::generate( entt::hashed_string string ) const
{
  string = string.data() ? string : "";

  for ( size_t i = 0;
        tags.count(string) > 0;
        ++i )
    string = entt::hashed_string( (string.data() + std::to_string(i)).c_str() );

  return string;
}

} // namespace cqde::types
