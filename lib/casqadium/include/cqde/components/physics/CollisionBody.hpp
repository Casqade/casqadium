#pragma once

#include <cqde/rp3d_fwd.hpp>

#include <cqde/types/physics/colliders/Collider.hpp>

#include <entt/fwd.hpp>
#include <entt/entity/entity.hpp>

#include <json/value.h>


namespace cqde::compos
{

struct CollisionBody
{
  using Collider = types::Collider;

  std::vector <std::shared_ptr <Collider>> colliders {};

  Json::Value bodyState {Json::objectValue};

  rp3d::PhysicsWorld* world {};
  rp3d::CollisionBody* body {};


  CollisionBody();
  ~CollisionBody();

  void enable( entt::registry& );
  void disable();


  void ui_edit_props( const entt::entity,
                      const entt::registry& );

  Json::Value serialize() const;

  static void deserialize(
    entt::registry&,
    entt::entity,
    const Json::Value&,
    const std::unordered_map <EntityId, EntityId,
                              identifier_hash>& idMap = {} );
};

} // namespace cqde::compos
