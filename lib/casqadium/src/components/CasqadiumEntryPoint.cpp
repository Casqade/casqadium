#include <cqde/components/CasqadiumEntryPoint.hpp>

#include <entt/entity/registry.hpp>

#include <json/value.h>


namespace cqde::compos
{

Json::Value
CasqadiumEntryPoint::serialize() const
{
  return Json::ValueType::objectValue;
}

void
CasqadiumEntryPoint::deserialize(
  entt::registry& registry,
  entt::entity entity,
  const Json::Value& json,
  const std::unordered_map <EntityId, EntityId,
                            identifier_hash>& idMap )
{
  registry.emplace_or_replace <CasqadiumEntryPoint> (entity);
}

} // namespace cqde::compos
