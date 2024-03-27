#include <cqde/components/audio/AudioListener3d.hpp>

#include <entt/entity/registry.hpp>

#include <json/value.h>


namespace cqde::compos
{

Json::Value
AudioListener3d::serialize() const
{
  return Json::ValueType::objectValue;
}

void
AudioListener3d::deserialize(
  entt::registry& registry,
  entt::entity entity,
  const Json::Value&,
  const IdMap& idMap )
{
  registry.emplace_or_replace <AudioListener3d> (entity);
}

} // namespace cqde::compos
