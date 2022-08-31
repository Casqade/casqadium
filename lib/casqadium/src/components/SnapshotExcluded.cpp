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
  const Json::Value& )
{
  registry.emplace <SnapshotExcluded> (entity);
}

} // namespace cqde::compos
