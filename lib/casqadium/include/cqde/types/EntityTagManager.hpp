#pragma once

#include <cqde/alias.hpp>
#include <cqde/common.hpp>

#include <map>


namespace cqde::types
{

struct EntityTagManager
{
  std::map <EntityId, entt::entity> tags;

  EntityTagManager();

  entt::entity get( const EntityId& ) const;
  EntityId generate( EntityId hint = {"e_"} ) const;
};

} // namespace cqde::types
