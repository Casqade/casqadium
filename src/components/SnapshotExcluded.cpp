#include <cqde/components/SnapshotExcluded.hpp>

#include <entt/entity/registry.hpp>

#include <json/value.h>


namespace cqde::compos
{

Json::Value
SnapshotExcluded::serialize() const
{
  return Json::ValueType::objectValue;
}

void
SnapshotExcluded::deserialize(
  entt::registry& registry,
  entt::entity entity,
  const Json::Value&,
  const IdMap& idMap )
{
  registry.emplace_or_replace <SnapshotExcluded> (entity);
}

} // namespace cqde::compos
