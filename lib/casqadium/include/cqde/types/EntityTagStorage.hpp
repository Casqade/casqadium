#pragma once

#include <cqde/alias.hpp>
#include <cqde/common.hpp>

#include <map>


namespace cqde::types
{

struct EntityTagStorage
{
  std::map <cqde::EntityId, entt::entity,
            entt::hashed_string_comparator> tags;

  EntityTagStorage();

  entt::entity get( const entt::hashed_string& ) const;
  entt::hashed_string generate( entt::hashed_string ) const;
};

} // namespace cqde::types
