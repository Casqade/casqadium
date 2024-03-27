#pragma once

#include <cqde/rp3d_fwd.hpp>

#include <cqde/types/physics/colliders/Collider.hpp>

#include <entt/fwd.hpp>

#include <glm/mat4x4.hpp>

#include <json/value.h>


namespace cqde::compos
{

struct RigidBody
{
  using Collider = types::Collider;

  glm::mat4 transformPrevious {1.0f};

  std::vector <std::shared_ptr <Collider>> colliders {};

  rp3d::PhysicsWorld* world {};
  rp3d::RigidBody* body {};

  bool massPropsFromColliders {};


  RigidBody() = default;
  RigidBody( RigidBody&& );
  RigidBody& operator = ( RigidBody&& );

  ~RigidBody();


  static std::string BodyTypeToString( const rp3d::BodyType );
  static rp3d::BodyType BodyTypeFromString( const std::string& );


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
