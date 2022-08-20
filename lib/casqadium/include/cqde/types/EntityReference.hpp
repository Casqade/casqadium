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
  EntityReference( const EntityId& );
  EntityReference( const compos::Tag& );

  entt::entity get( const entt::registry& ) const;
  entt::entity get_if_valid( const entt::registry& ) const;

  bool operator < ( const EntityReference& ) const;
};

} // namespace cqde::types
