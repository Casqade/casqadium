#include <cqde/components/CasqadiumEditorCameraSettings.hpp>

#include <entt/entity/registry.hpp>

#include <json/value.h>


namespace cqde::compos
{

Json::Value
CasqadiumEditorCameraSettings::serialize() const
{
  return Json::ValueType::objectValue;
}

void
CasqadiumEditorCameraSettings::deserialize(
  entt::registry& registry,
  entt::entity entity,
  const Json::Value&,
  const std::unordered_map <EntityId, EntityId,
                            identifier_hash>& idMap )
{
  registry.emplace_or_replace <CasqadiumEditorCameraSettings> (entity);
}

} // namespace cqde::compos
