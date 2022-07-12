#include <cqde/types/SystemManager.hpp>

#include <cqde/util/logger.hpp>


namespace cqde::types
{

void
SystemManager::execute(
  entt::registry& registry,
  const bool rendering )
{
  if ( mSystemsToDisable.empty() == false )
  {
    for ( const auto& systemId : mSystemsToDisable )
    {
      mSystemsEnabled.erase(systemId);
      mRenderSystemsEnabled.erase(systemId);
    }
    mSystemsToDisable.clear();
  }

  auto& systemsPool = rendering == true
                    ? mRenderSystemsEnabled
                    : mSystemsEnabled;

  for ( const auto& [systemId, system] : systemsPool )
    system(registry);
}

void
SystemManager::enable(
  const SystemId& systemId,
  const bool rendering )
{
  LOG_DEBUG("Enabling system '{}'",
            systemId.str());

  if ( mSystemsPool.count(systemId) == 0 )
    throw std::runtime_error(
      fmt::format("Can't enable unknown system '{}'",
                  systemId.str()));

  auto& systemsPool = rendering == true
                    ? mRenderSystemsEnabled
                    : mSystemsEnabled;

  systemsPool[systemId] = mSystemsPool[systemId];
}

void
SystemManager::disable(
  const SystemId& systemId )
{
  LOG_DEBUG("Disabling system '{}'",
            systemId.str());

  mSystemsToDisable.insert(systemId);
}

void
SystemManager::Register(
  const SystemId&   systemId,
  const SystemFunc& system )
{
  LOG_INFO("Registering system '{}'",
            systemId.str());

  mSystemsPool[systemId] = system;
}

std::set <SystemId>
SystemManager::systems() const
{
  std::set <SystemId> result {};

  for ( const auto& [systemId, system] : mSystemsPool )
    result.insert(systemId);

  return result;
}

std::set <SystemId>
SystemManager::systemsEnabled(
  const bool rendering ) const
{
  const auto& systemsPool = rendering == true
                          ? mRenderSystemsEnabled
                          : mSystemsEnabled;

  std::set <SystemId> result {};

  for ( const auto& [systemId, system] : systemsPool )
    if ( mSystemsToDisable.count(systemId) == 0 )
      result.insert(systemId);

  return result;
}

} // namespace cqde::types
