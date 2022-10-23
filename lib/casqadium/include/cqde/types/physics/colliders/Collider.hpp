#pragma once

#include <cqde/alias.hpp>
#include <cqde/rp3d_fwd.hpp>

#include <entt/fwd.hpp>

#include <json/value.h>


namespace cqde::types
{

struct Collider
{
  struct
  {
    std::vector <CallbackId> onEnter {};
    std::vector <CallbackId> onStay {};
    std::vector <CallbackId> onLeave {};

  } callbacks {};

protected:

  rp3d::PhysicsCommon* mCommon {};
  rp3d::Collider* mCollider {};

  Json::Value mState {Json::objectValue};

public:
  Collider();
  virtual ~Collider();

  void enable( entt::registry&, rp3d::CollisionBody* );
  void disable();

  void destroy();

  void onEnter( const entt::registry&,
                const entt::entity body1,
                const entt::entity body2 );

  void onStay(  const entt::registry&,
                const entt::entity body1,
                const entt::entity body2 );

  void onLeave( const entt::registry&,
                const entt::entity body1,
                const entt::entity body2 );

  virtual void shapeEnable();
  virtual void shapeDisable();

  virtual void ui_show( const entt::registry& );

  void deserialize( const Json::Value& );
  virtual Json::Value serialize() const;

  void stateApply();
  void stateValidate();

  virtual void shapeStateApply();
  virtual void shapeStateValidate();

  virtual rp3d::Collider* collider() const;
  virtual rp3d::CollisionShape* shape() const;

  virtual std::string type() const;
};

} // namespace cqde::types
