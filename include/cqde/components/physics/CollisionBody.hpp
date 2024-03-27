#pragma once

#include <cqde/rp3d_fwd.hpp>

#include <cqde/types/physics/colliders/Collider.hpp>

#include <entt/fwd.hpp>

#include <json/value.h>


namespace cqde::compos
{

struct CollisionBody
{
  using Collider = types::Collider;

  std::vector <std::shared_ptr <Collider>> colliders {};

  rp3d::PhysicsWorld* world {};
  rp3d::CollisionBody* body {};


  CollisionBody() = default;
  CollisionBody( CollisionBody&& );
  CollisionBody& operator = ( CollisionBody&& );

  ~CollisionBody();


  void ui_edit_props( const entt::entity,
                      entt::registry& );

  Json::Value serialize() const;

  static void deserialize(
    entt::registry&,
    entt::entity,
    const Json::Value&,
    const IdMap& = {} );
};

} // namespace cqde::compos
