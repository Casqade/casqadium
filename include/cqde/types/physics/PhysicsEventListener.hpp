#pragma once

#include <cqde/rp3d_fwd.hpp>

#include <entt/fwd.hpp>

#include <reactphysics3d/engine/EventListener.h>


namespace cqde::types
{

class PhysicsEventListener : public rp3d::EventListener
{
  using CollisionCallback = rp3d::CollisionCallback;
  using OverlapCallback = rp3d::OverlapCallback;

  entt::registry& mRegistry;

  entt::entity bodyToEntity( const rp3d::CollisionBody* ) const;

public:
  PhysicsEventListener( entt::registry& );

  void onContact( const CollisionCallback::CallbackData& data ) override;
  void onTrigger( const OverlapCallback::CallbackData& data ) override;
};

} // namespace cqde::types
