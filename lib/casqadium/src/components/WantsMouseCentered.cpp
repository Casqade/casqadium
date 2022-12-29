#include <cqde/components/WantsMouseCentered.hpp>

#include <entt/entity/registry.hpp>

#include <json/value.h>


namespace cqde::compos
{

Json::Value
WantsMouseCentered::serialize() const
{
  return Json::ValueType::objectValue;
}

void
WantsMouseCentered::deserialize(
  entt::registry& registry,
  entt::entity entity,
  const Json::Value&,
  const std::unordered_map <EntityId, EntityId,
                            identifier_hash>& idMap )
{
  registry.emplace_or_replace <WantsMouseCentered> (entity);
}

} // namespace cqde::compos
