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
  const Json::Value&,
  const std::unordered_map <EntityId, EntityId,
                            identifier_hash>& idMap )
{
  registry.emplace <SubscriberUpdate> (entity);
}

void
SubscriberUpdate::ui_edit_props(
  const entt::entity,
  const entt::registry& )
{}

} // namespace cqde::compos
