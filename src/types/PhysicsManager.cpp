#include <cqde/types/PhysicsManager.hpp>

#include <cqde/logger.hpp>


namespace cqde::types
{

void
PhysicsLogger::log(
  Level level,
  const std::string& physicsWorldName,
  Category category,
  const std::string& message,
  const char* filename,
  int lineNumber )
{
  using fmt::format;
  using spdlog::level::level_enum;

  if ( level > mLevel )
    return;

  level_enum spdlog_level {};

  switch (level)
  {
    case Level::Error:
      spdlog_level = level_enum::err;
      break;

    case Level::Warning:
      spdlog_level = level_enum::warn;
      break;

    case Level::Information:
      spdlog_level = level_enum::trace;
      break;
  }

  spdlog::log(
    {filename, lineNumber, {}},
    spdlog_level,
    format("[{}] {}",
           getCategoryName(category),
           message));
}

void
PhysicsLogger::setLevel(
  const Level level )
{
  mLevel = level;
}

PhysicsLogger::Level
PhysicsLogger::level() const
{
  return mLevel;
}


PhysicsManager::PhysicsManager(
  entt::registry& registry )
  : mEventListener{registry}
{
  mCommon.setLogger(&mLogger);

  PhysicsWorld::WorldSettings settings {};
  settings.worldName = "casqadium";
  settings.gravity = {};
  settings.persistentContactDistanceThreshold = 0.03;
  settings.defaultFrictionCoefficient = 0.3;
  settings.defaultBounciness = 0.5;
  settings.restitutionVelocityThreshold = 0.5;
  settings.cosAngleSimilarContactManifold = 0.95;

  mWorld = mCommon.createPhysicsWorld(settings);
  mWorld->setEventListener(&mEventListener);
  mWorld->setIsGravityEnabled(false);
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
