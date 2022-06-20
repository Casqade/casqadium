#include <cqde/types/EntityTagManager.hpp>

#include <entt/entity/entity.hpp>

#include <string>


namespace cqde::types
{

EntityTagManager::EntityTagManager()
  : tags({{cqde::null_id, entt::null}})
{}

entt::entity
EntityTagManager::get( const EntityId& id ) const
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
EntityTagManager::generate( EntityId hint ) const
{
  size_t idInc {};

  while ( tags.count(EntityId{(hint.str() + std::to_string(idInc)).c_str()}) > 0 )
    ++idInc;

  return EntityId{(hint.str() + std::to_string(idInc)).c_str()};
}

} // namespace cqde::types
