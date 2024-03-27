#include <cqde/components/SubscriberInput.hpp>

#include <entt/entity/registry.hpp>

#include <json/value.h>


namespace cqde::compos
{

Json::Value
SubscriberInput::serialize() const
{
  return Json::ValueType::objectValue;
}

void
SubscriberInput::deserialize(
  entt::registry& registry,
  entt::entity entity,
  const Json::Value&,
  const IdMap& idMap )
{
  registry.emplace_or_replace <SubscriberInput> (entity);
}

} // namespace cqde::compos
