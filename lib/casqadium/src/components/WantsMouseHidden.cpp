#include <cqde/components/WantsMouseHidden.hpp>

#include <entt/entity/registry.hpp>

#include <json/value.h>


namespace cqde::compos
{

Json::Value
WantsMouseHidden::serialize() const
{
  return Json::ValueType::objectValue;
}

void
WantsMouseHidden::deserialize(
  entt::registry& registry,
  entt::entity entity,
  const Json::Value&,
  const std::unordered_map <EntityId, EntityId,
                            identifier_hash>& idMap )
{
  registry.emplace_or_replace <WantsMouseHidden> (entity);
}

} // namespace cqde::compos
