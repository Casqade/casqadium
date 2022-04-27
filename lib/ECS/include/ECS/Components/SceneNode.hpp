#pragma once

#include <ECS/Types/EntityReference.hpp>

#include <set>
#include <string>


namespace ECS
{

namespace Components
{

struct SceneNode
{
  Types::EntityReference parent;
  std::set <Types::EntityReference> children;


  SceneNode();


  void serialize( const std::string& json ) const;

  void deserialize( entt::registry&,
                    entt::entity,
                    const std::string& json ) const;

  static void Register();
};

void AttachChildNode( entt::registry&, entt::entity parent, entt::entity child );
entt::entity DetachChildNode( entt::registry&, entt::entity parent, entt::entity child );
void RemoveChildNode( entt::registry&, entt::entity parent, entt::entity child );

} // namespace Components

} // namespace ECS
