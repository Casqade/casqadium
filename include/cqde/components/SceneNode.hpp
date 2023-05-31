#pragma once

#include <cqde/types/EntityReference.hpp>

#include <json/forwards.h>

#include <unordered_map>
#include <unordered_set>


namespace cqde::compos
{

struct SceneNode
{
  types::EntityReference parent {};
  std::unordered_set <types::EntityReference,
                      identifier_hash> children {};


  SceneNode() = default;


  void ui_edit_props( const entt::entity,
                      entt::registry& );

  Json::Value serialize() const;

  static void deserialize(
    entt::registry&,
    entt::entity,
    const Json::Value&,
    const std::unordered_map <EntityId, EntityId,
                              identifier_hash>& idMap = {} );
};

} // namespace cqde::compos

namespace cqde
{

size_t ChildNodeDepth(
  const entt::registry&,
  const compos::SceneNode* );

bool CanAddChildNode(
  const entt::registry&,
  const entt::entity parent,
  const EntityId& child );

void AttachChildNode(
  entt::registry&,
  entt::entity parent,
  entt::entity child );

void DetachChildNode(
  entt::registry&,
  entt::entity parent,
  entt::entity child );

void DestroyChildNode(
  entt::registry&,
  entt::entity parent,
  entt::entity child );

void RootifyChildNode(
  entt::registry&,
  const entt::entity );

void SerializeChildNode(
  const entt::registry&,
  Json::Value&,
  const entt::entity,
  const std::unordered_set <ComponentType>& exclude = {} );

} // namespace cqde
