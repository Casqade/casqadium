#include <cqde/components/Tag.hpp>
#include <cqde/types/EntityTagManager.hpp>

#include <entt/entity/registry.hpp>

#include <json/value.h>


namespace cqde::compos
{

void
Tag::invalidate( entt::registry& registry )
{
  using namespace entt::literals;

  if ( !(id == null_id) )
    registry.ctx().at <types::EntityTagManager> ().tags.erase(id);
}

Json::Value
Tag::serialize() const
{
  return Json::ValueType::objectValue;
}

void
Tag::deserialize(
  entt::registry&,
  entt::entity,
  const Json::Value& )
{}

} // namespace cqde::compos
