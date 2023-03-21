#pragma once

#include <cqde/alias.hpp>
#include <cqde/rp3d_fwd.hpp>

#include <entt/fwd.hpp>

#include <json/value.h>

#include <any>
#include <vector>


namespace cqde::types
{

class Collider
{
  struct
  {
    std::vector <CallbackId> onEnter {};
    std::vector <CallbackId> onStay {};
    std::vector <CallbackId> onLeave {};

  } mCallbacks {};

protected:
  rp3d::PhysicsCommon* mCommon {};
  rp3d::Collider* mCollider {};

  void destroy();

  virtual void shapeInit( entt::registry& );
  virtual void shapeDestroy();

  virtual Json::Value shapeSerialize() const;

  virtual void shapeDeserialize( entt::registry&, const Json::Value& );

public:
  Collider() = default;
  virtual ~Collider();

  void init( entt::registry&, rp3d::CollisionBody* );

  void onEnter( entt::registry&,
                const std::vector <std::any>& args );

  void onStay(  entt::registry&,
                const std::vector <std::any>& args );

  void onLeave( entt::registry&,
                const std::vector <std::any>& args );


  virtual void ui_show( const entt::registry& );

  Json::Value serialize() const;

  void deserialize( entt::registry&,
                    rp3d::CollisionBody*,
                    const Json::Value& );


  rp3d::Collider* collider() const;

  virtual rp3d::CollisionShape* shape() const;
  virtual std::string type() const;
};

} // namespace cqde::types
