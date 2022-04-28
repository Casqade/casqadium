#pragma once

#include <cqde/alias.hpp>
#include <cqde/common.hpp>

#include <map>


namespace cqde::types
{

struct EntityTagStorage
{
  std::map <EntityId, entt::entity,
            entt::hashed_string_comparator> tags;

  EntityTagStorage();

  entt::entity get( const EntityId& ) const;
  EntityId generate( EntityId hint = {"entity_"} ) const;
};

} // namespace cqde::types
