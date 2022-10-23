#include <cqde/types/PhysicsManager.hpp>


namespace cqde::types
{

PhysicsManager::PhysicsManager(
  entt::registry& registry )
  : mEventListener{registry}
{
  mCommon.setLogger(&mLogger);

  PhysicsWorld::WorldSettings settings {};
  settings.worldName = "casqadium";
  settings.persistentContactDistanceThreshold = 0.03;
  settings.defaultFrictionCoefficient = 0.3;
  settings.defaultBounciness = 0.5;
  settings.restitutionVelocityThreshold = 0.5;
  settings.cosAngleSimilarContactManifold = 0.95;

  mWorld = mCommon.createPhysicsWorld(settings);
  mWorld->setEventListener(&mEventListener);
}

PhysicsManager::PhysicsWorld*
PhysicsManager::world() const
{
  return mWorld;
}

PhysicsManager::PhysicsCommon&
PhysicsManager::common()
{
  return mCommon;
}

} // namespace cqde::types
