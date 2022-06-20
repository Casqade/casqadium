#include <cqde/types/EntityReference.hpp>
#include <cqde/types/EntityTagManager.hpp>
#include <cqde/components/Tag.hpp>

#include <entt/entt.hpp>


namespace cqde::types
{

EntityReference::EntityReference()
  : id("null")
{}

EntityReference::EntityReference( const compos::Tag& tag )
  : id(tag.id)
{}

entt::entity
EntityReference::get( entt::registry& registry ) const
{
  return registry.ctx().at <EntityTagManager> ().get(id);
}

bool
EntityReference::operator < ( const EntityReference& other ) const
{
  return id.hash().value() < other.id.hash().value();
}

} // namespace cqde::types
