#include <cqde/components/CasqadiumEditorInternal.hpp>

#include <entt/entity/registry.hpp>

#include <json/value.h>


namespace cqde::compos
{

Json::Value
CasqadiumEditorInternal::serialize() const
{
  return Json::ValueType::objectValue;
}

void
CasqadiumEditorInternal::deserialize(
  entt::registry& registry,
  entt::entity entity,
  const Json::Value&,
  const IdMap& idMap )
{
  registry.emplace_or_replace <CasqadiumEditorInternal> (entity);
}

} // namespace cqde::compos
