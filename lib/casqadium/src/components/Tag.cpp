#include <cqde/components/Tag.hpp>
#include <cqde/types/EntityManager.hpp>

#include <entt/entity/registry.hpp>

#include <json/value.h>


namespace cqde::compos
{

void
Tag::invalidate( entt::registry& registry ) const
{
  registry.ctx().at <types::EntityManager> ().idInvalidate(id);
}

Json::Value
Tag::serialize() const
{
  return Json::ValueType::objectValue;
}

void
Tag::deserialize(
  entt::registry&,
  entt::entity,
  const Json::Value&,
  const std::unordered_map <EntityId, EntityId,
                            identifier_hash>& idMap )
{}

} // namespace cqde::compos
