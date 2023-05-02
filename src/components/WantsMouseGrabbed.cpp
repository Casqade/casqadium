#include <cqde/components/WantsMouseGrabbed.hpp>

#include <entt/entity/registry.hpp>

#include <json/value.h>


namespace cqde::compos
{

Json::Value
WantsMouseGrabbed::serialize() const
{
  return Json::ValueType::objectValue;
}

void
WantsMouseGrabbed::deserialize(
  entt::registry& registry,
  entt::entity entity,
  const Json::Value&,
  const std::unordered_map <EntityId, EntityId,
                            identifier_hash>& idMap )
{
  registry.emplace_or_replace <WantsMouseGrabbed> (entity);
}

} // namespace cqde::compos
