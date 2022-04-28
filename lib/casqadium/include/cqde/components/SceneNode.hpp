#pragma once

#include <cqde/types/EntityReference.hpp>

#include <set>
#include <string>


namespace cqde::compos
{

struct SceneNode
{
  types::EntityReference parent;
  std::set <types::EntityReference> children;


  SceneNode();


  void serialize( const std::string& json ) const;

  void deserialize( entt::registry&,
                    entt::entity,
                    const std::string& json ) const;

  static void Register();
};

} // namespace cqde::compos

namespace cqde
{

void AttachChildNode( entt::registry&, entt::entity parent, entt::entity child );

entt::entity DetachChildNode( entt::registry&, entt::entity parent, entt::entity child );

void RemoveChildNode( entt::registry&, entt::entity parent, entt::entity child );

} // namespace cqde
