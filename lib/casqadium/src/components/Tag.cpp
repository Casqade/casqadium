#include <cqde/components/Tag.hpp>
#include <cqde/types/EntityTagStorage.hpp>

#include <entt/entt.hpp>
#include <entt/core/hashed_string.hpp>

#include <json/value.h>


namespace cqde::compos
{

void
Tag::serialize( Json::Value& json ) const
{
}

void
Tag::deserialize(
  entt::registry& registry,
  entt::entity entity,
  const Json::Value& content ) const
{
  auto& comp = registry.emplace <Tag> (entity);

//  if ( idStorage.emplace(comp.id.value(), entity).second == false )
//    throw "duplicate id encountered";
}

void
Tag::Register()
{
  using namespace entt::literals;

  auto factory = entt::meta <Tag> ();
  factory.type(entt::hashed_string{"Tag"});
  factory.data <&Tag::id> ("id"_hs);
  factory.func <&Tag::serialize> ("serialize"_hs);
  factory.func <&Tag::deserialize> ("deserialize"_hs);
}

Tag
Tag::Generate(
  entt::registry& registry,
  entt::hashed_string string )
{
  return
  {
    registry.ctx <types::EntityTagStorage> ().generate(string)
  };
}

} // namespace cqde::compos
