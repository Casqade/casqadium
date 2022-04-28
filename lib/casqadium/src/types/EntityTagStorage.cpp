#include <cqde/types/EntityTagStorage.hpp>

#include <entt/entity/entity.hpp>

#include <string>


namespace cqde::types
{

EntityTagStorage::EntityTagStorage()
  : tags({{cqde::null_id, entt::null}})
{}

entt::entity
EntityTagStorage::get( const EntityId& id ) const
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

EntityId
EntityTagStorage::generate( EntityId hint ) const
{
  hint = hint.data() ? hint : "";

  std::string idBase {hint.data()};
  size_t      idInc {};

  while ( tags.count(EntityId{(idBase + std::to_string(idInc)).c_str()}) > 0 )
    ++idInc;

  return EntityId{(idBase + std::to_string(idInc)).c_str()};
}

} // namespace cqde::types
