#include <cqde/types/SystemManager.hpp>

#include <cqde/util/logger.hpp>


namespace cqde::types
{

SystemManager::SystemsIterator
SystemManager::systemIter(
  const SystemId& id )
{
  const auto predicate =
  [&id] ( const System& system )
  {
    return system.id == id;
  };

  return std::find_if(
    mSystems.begin(),
    mSystems.end(),
    predicate);
}

void
SystemManager::execute(
  entt::registry& registry,
  const Phase phase )
{
  for ( const auto& system : mSystems )
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

  if ( const auto system = systemIter(systemId);
       system != mSystems.end() )
  {
    system->active = true;
    return;
  }

  throw std::runtime_error(
    fmt::format("Failed to activate unknown system '{}'",
                systemId.str()));
}

void
SystemManager::deactivate(
  const SystemId& systemId )
{
  LOG_DEBUG("Deactivating system '{}'",
            systemId.str());

  if ( const auto system = systemIter(systemId);
       system != mSystems.end() )
  {
    system->active = false;
    return;
  }

  throw std::runtime_error(
    fmt::format("Failed to deactivate unknown system '{}'",
                systemId.str()));
}

void
SystemManager::Register(
  const SystemId& systemId,
  const Callback& callback,
  const Phase     phase )
{
  LOG_INFO("Registering system '{}'",
            systemId.str());

  mSystems.insert(systemIter(systemId),
                  {callback, systemId,
                  phase, false} );
}

std::vector <SystemId>
SystemManager::systems() const
{
  std::vector <SystemId> result {};

  for ( const auto& system : mSystems )
    result.push_back(system.id);

  return result;
}

std::vector <SystemId>
SystemManager::systemsActive(
  const Phase phase ) const
{
  std::vector <SystemId> result {};

  for ( const auto& system : mSystems )
    if ( system.active == true &&
         system.phase == phase )
      result.push_back(system.id);

  return result;
}

} // namespace cqde::types
