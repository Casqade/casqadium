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
  const Json::Value& )
{
  registry.emplace <SubscriberInput> (entity);
}

} // namespace cqde::compos
