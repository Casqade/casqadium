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
  const std::unordered_map <EntityId, EntityId,
                            identifier_hash>& idMap )
{
  registry.emplace <SnapshotExcluded> (entity);
}

void
SnapshotExcluded::ui_edit_props(
  const entt::entity,
  const entt::registry& )
{}

} // namespace cqde::compos
