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
  return registry.ctx().at <EntityManager> ().get(id);
}

entt::entity
EntityReference::get_if_valid(
  const entt::registry& registry ) const
{
  const auto& entityManager = registry.ctx().at <EntityManager> ();
  return entityManager.get_if_valid(id, registry);
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
