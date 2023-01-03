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
  if ( entity_valid(entity, registry) == false )
  {
    const auto& entityManager = registry.ctx().at <EntityManager> ();
    entity = entityManager.get_if_valid(id, registry);
  }

  return entity;
}

bool
EntityReference::operator == (
  const EntityReference& other ) const
{
  return id == other.id;
}

bool
EntityReference::operator < (
  const EntityReference& other ) const
{
  return id < other.id;
}

} // namespace cqde::types
