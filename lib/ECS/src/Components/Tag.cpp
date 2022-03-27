#include <ECS/Components/Tag.hpp>

#include <entt/entt.hpp>


namespace ECS
{

namespace Components
{


std::map <ENTT_ID_TYPE, entt::entity>&
entityIdStorage()
{
  static std::unique_ptr <std::map <ENTT_ID_TYPE, entt::entity>> storage = std::make_unique <std::map <ENTT_ID_TYPE, entt::entity>> ();
  if ( storage == nullptr )
    storage = std::make_unique <std::map <ENTT_ID_TYPE, entt::entity>> ();

  return *storage.get();
}

Tag::Tag()
  : id()
{}

void
Tag::serialize( const std::string& json ) const
{}

void
Tag::deserialize(
  entt::registry& registry,
  entt::entity entity,
  const std::string& content ) const
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
  factory.func <&Tag::serialize> ("serialze"_hs);
  factory.func <&Tag::deserialize> ("deserialze"_hs);
}

EntityReference::EntityReference()
  : id()
{}

entt::entity
EntityReference::get()
{
  if ( id.data() != nullptr && entityIdStorage().count(id.value()) > 0 )
    return entityIdStorage()[id.value()];

  return entt::null;
}

Tag
Tag::Generate( entt::hashed_string string )
{
  string = string.data() ? string : "";

  for ( size_t i = 0; entityIdStorage().count(string.value()) > 0; ++i )
    string = entt::hashed_string( (string.data() + std::to_string(i)).c_str() );

  Tag tag;
  tag.id = string;

  return tag;
}

} // namespace Components

} // namespace ECS
