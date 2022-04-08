#ifndef ECS_TYPES_ENTITY_REFERENCE_HPP
#define ECS_TYPES_ENTITY_REFERENCE_HPP

#include <ECS/Alias.hpp>

#include <entt/fwd.hpp>


namespace ECS
{

namespace Components
{
struct Tag;
}

namespace Types
{

struct EntityReference
{
  EntityId id;


  EntityReference();
  EntityReference( const Components::Tag& );

  entt::entity get() const;

  bool operator < ( const EntityReference& ) const;
};

} // namespace Types


} // namespace ECS

#endif

