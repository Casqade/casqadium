#include <cqde/components/Tag.hpp>
#include <cqde/types/EntityTagManager.hpp>

#include <entt/entt.hpp>
#include <entt/core/hashed_string.hpp>

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

void
Tag::serialize( Json::Value& json ) const
{
  json["id"] = id.str();
}

void
Tag::deserialize(
  entt::registry& registry,
  entt::entity entity,
  const Json::Value& content ) const
{
  auto& comp = registry.emplace <Tag> (entity);

  comp.id = EntityId{content.get("id", null_id.str()).asCString()};

  if ( registry.ctx().at <types::EntityTagManager> ().tags.emplace(comp.id, entity).second == false )
    throw "duplicate id encountered";
}

void
Tag::Register()
{
  using namespace entt::literals;

  auto factory = entt::meta <Tag> ();
  factory.type("Tag"_hs);
  factory.data <&Tag::id> ("id"_hs);
  factory.func <&Tag::serialize> ("serialize"_hs);
  factory.func <&Tag::deserialize> ("deserialize"_hs);
}

} // namespace cqde::compos
