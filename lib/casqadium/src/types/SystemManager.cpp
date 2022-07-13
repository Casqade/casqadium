#include <cqde/types/SystemManager.hpp>

#include <cqde/util/logger.hpp>


namespace cqde::types
{

void
SystemManager::execute(
  entt::registry& registry,
  const Phase phase )
{
  for ( const auto& [systemId, system] : mSystems )
    if ( system.active == true &&
         system.phase == phase )
      system.callback(registry);
}

void
SystemManager::activate(
  const SystemId& systemId )
{
  LOG_DEBUG("Activating system '{}'",
            systemId.str());

  if ( mSystems.count(systemId) == 0 )
    throw std::runtime_error(
      fmt::format("Failed to activate unknown system '{}'",
                  systemId.str()));

  mSystems[systemId].active = true;
}

void
SystemManager::deactivate(
  const SystemId& systemId )
{
  LOG_DEBUG("Deactivating system '{}'",
            systemId.str());

  if ( mSystems.count(systemId) == 0 )
    throw std::runtime_error(
      fmt::format("Failed to deactivate unknown system '{}'",
                  systemId.str()));

  mSystems[systemId].active = false;
}

void
SystemManager::Register(
  const SystemId& systemId,
  const Callback& callback,
  const Phase     phase )
{
  LOG_INFO("Registering system '{}'",
            systemId.str());

  mSystems[systemId] = System {callback, phase, false};
}

std::vector <SystemId>
SystemManager::systems() const
{
  std::vector <SystemId> result {};

  for ( const auto& [systemId, system] : mSystems )
    result.push_back(systemId);

  return result;
}

std::vector <SystemId>
SystemManager::systemsActive(
  const Phase phase ) const
{
  std::vector <SystemId> result {};

  for ( const auto& [systemId, system] : mSystems )
    if ( system.active == true &&
         system.phase == phase )
      result.push_back(systemId);

  return result;
}

} // namespace cqde::types
