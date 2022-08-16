#include <cqde/types/EntityReference.hpp>
#include <cqde/types/EntityManager.hpp>
#include <cqde/components/Tag.hpp>

#include <entt/entt.hpp>


namespace cqde::types
{

EntityReference::EntityReference(
  const EntityId& id )
  : id{id}
{}

EntityReference::EntityReference(
  const compos::Tag& tag )
  : id{tag.id}
{}

entt::entity
EntityReference::get(
  const entt::registry& registry ) const
{
  const auto entity = registry.ctx().at <EntityManager> ().get(id);

  if ( registry.valid(entity) == true )
    return entity;

  return entt::null;
}

bool
EntityReference::operator < (
  const EntityReference& other ) const
{
  return id.hash().value() < other.id.hash().value();
}

} // namespace cqde::types
