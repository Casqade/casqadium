#pragma once

#include <cqde/types/EntityReference.hpp>

#include <json/forwards.h>

#include <set>


namespace cqde::compos
{

struct SceneNode
{
  types::EntityReference parent {};
  std::set <types::EntityReference> children {};


  SceneNode() = default;


  void ui_edit_props( const entt::entity,
                      const entt::registry& );

  Json::Value serialize() const;

  static void deserialize(
    entt::registry&,
    entt::entity,
    const Json::Value& );
};

} // namespace cqde::compos

namespace cqde
{

void AttachChildNode( entt::registry&, entt::entity parent, entt::entity child );

entt::entity DetachChildNode( entt::registry&, entt::entity parent, entt::entity child );

void RemoveChildNode( entt::registry&, entt::entity parent, entt::entity child );

} // namespace cqde
