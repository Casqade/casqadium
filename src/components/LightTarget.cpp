#include <cqde/components/LightTarget.hpp>

#include <entt/entity/registry.hpp>

#include <json/value.h>


namespace cqde::compos
{

Json::Value
LightTarget::serialize() const
{
  return Json::ValueType::objectValue;
}

void
LightTarget::deserialize(
  entt::registry& registry,
  entt::entity entity,
  const Json::Value&,
  const std::unordered_map <EntityId, EntityId,
                            identifier_hash>& idMap )
{
  registry.emplace_or_replace <LightTarget> (entity);
}

} // namespace cqde::compos
