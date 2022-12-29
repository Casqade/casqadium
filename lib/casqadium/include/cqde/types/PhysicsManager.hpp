#pragma once

#include <cqde/rp3d_fwd.hpp>
#include <cqde/util/logger_rp3d.hpp>

#include <cqde/types/physics/PhysicsEventListener.hpp>

#include <entt/fwd.hpp>

#include <reactphysics3d/engine/PhysicsCommon.h>


namespace cqde::types
{

class PhysicsManager
{
  using PhysicsCommon = rp3d::PhysicsCommon;
  using PhysicsWorld = rp3d::PhysicsWorld;

  PhysicsEventListener mEventListener;

  logger_rp3d mLogger {};

  PhysicsCommon mCommon {};
  PhysicsWorld* mWorld {};

public:
  PhysicsManager( entt::registry& );

  PhysicsWorld* world() const;
  PhysicsCommon& common();
};

} // namespace cqde::types
