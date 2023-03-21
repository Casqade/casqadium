#pragma once

#include <cqde/alias.hpp>
#include <cqde/fwd.hpp>

#include <entt/entity/entity.hpp>


namespace cqde::types
{

struct EntityReference
{
  EntityId id {};
  mutable entt::entity entity {entt::null};


  EntityReference() = default;
  EntityReference( const EntityId& );
  EntityReference( const compos::Tag& );

  entt::entity get( const entt::registry& ) const;

  bool operator == ( const EntityReference& ) const;
  bool operator < ( const EntityReference& ) const;
};

} // namespace cqde::types
