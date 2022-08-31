#include <cqde/components/SubscriberUpdate.hpp>

#include <entt/entity/registry.hpp>

#include <json/value.h>


namespace cqde::compos
{

Json::Value
SubscriberUpdate::serialize() const
{
  return Json::ValueType::objectValue;
}

void
SubscriberUpdate::deserialize(
  entt::registry& registry,
  entt::entity entity,
  const Json::Value& )
{
  registry.emplace <SubscriberUpdate> (entity);
}

} // namespace cqde::compos
