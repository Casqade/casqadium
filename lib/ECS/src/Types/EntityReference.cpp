#include <ECS/Types/EntityReference.hpp>
#include <ECS/Components/Tag.hpp>

#include <entt/entt.hpp>


namespace ECS
{

namespace Types
{


std::map <ENTT_ID_TYPE, entt::entity>&
entityTagStorage()
{
  static std::unique_ptr <std::map <ENTT_ID_TYPE, entt::entity>> storage = std::make_unique <std::map <ENTT_ID_TYPE, entt::entity>> ();
  if ( storage == nullptr )
    storage = std::make_unique <std::map <ENTT_ID_TYPE, entt::entity>> ();

  return *storage.get();
}


EntityReference::EntityReference()
  : id("null")
{}

EntityReference::EntityReference( const Components::Tag& tag )
  : id(tag.id)
{}

entt::entity
EntityReference::get() const
{
  if ( id.data() != nullptr && entityTagStorage().count(id.value()) > 0 )
    return entityTagStorage()[id.value()];

  return entt::null;
}

bool
EntityReference::operator < ( const EntityReference& other ) const
{
  return id.value() < other.id.value();
}

} // namespace Components

} // namespace ECS

