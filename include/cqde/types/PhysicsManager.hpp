#pragma once

#include <cqde/rp3d_fwd.hpp>

#include <cqde/types/physics/PhysicsEventListener.hpp>

#include <entt/fwd.hpp>

#include <reactphysics3d/engine/PhysicsCommon.h>

#include <spdlog/common.h>


namespace cqde::types
{

class PhysicsLogger : public rp3d::Logger
{
  Level mLevel {Level::Warning};

public:
  ~PhysicsLogger() override = default;

  void log(
    Level,
    const std::string& physicsWorldName,
    Category,
    const std::string& message,
    const char* filename,
    int lineNumber ) override;

  void setLevel( const Level );
  Level level() const;
};


class PhysicsManager
{
  using PhysicsCommon = rp3d::PhysicsCommon;
  using PhysicsWorld = rp3d::PhysicsWorld;

  PhysicsEventListener mEventListener;

  PhysicsLogger mLogger {};

  PhysicsCommon mCommon {};
  PhysicsWorld* mWorld {};

public:
  PhysicsManager( entt::registry& );

  PhysicsWorld* world() const;
  PhysicsCommon& common();
};

} // namespace cqde::types
