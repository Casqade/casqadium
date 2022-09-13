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
  const std::unordered_map <EntityId, EntityId,
                            identifier_hash>& idMap )
{}

void
CasqadiumEditorInternal::ui_edit_props(
  const entt::entity,
  const entt::registry& )
{}

} // namespace cqde::compos
