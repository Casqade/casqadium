#pragma once

#include <cqde/alias.hpp>
#include <cqde/fwd.hpp>

#include <entt/fwd.hpp>


namespace cqde::types
{

struct EntityReference
{
  EntityId id {};


  EntityReference() = default;
  EntityReference( const compos::Tag& );

  entt::entity get( entt::registry& ) const;

  bool operator < ( const EntityReference& ) const;
};

} // namespace cqde::types
