#include <cqde/components/Tag.hpp>
#include <cqde/types/EntityManager.hpp>

#include <entt/entity/registry.hpp>

#include <json/value.h>


namespace cqde::compos
{

void
Tag::invalidate( entt::registry& registry )
{
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
  const Json::Value& )
{}

} // namespace cqde::compos
