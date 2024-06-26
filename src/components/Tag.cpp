#include <cqde/components/Tag.hpp>
#include <cqde/types/EntityManager.hpp>

#include <entt/entity/registry.hpp>

#include <json/value.h>


namespace cqde::compos
{

void
Tag::invalidate( entt::registry& registry ) const
{
  registry.ctx().get <types::EntityManager> ().idInvalidate(id);
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
  const IdMap& idMap )
{}

} // namespace cqde::compos
