#include <cqde/types/SystemManager.hpp>


namespace cqde::types
{

void
SystemManager::execute(
  entt::registry& registry,
  const bool rendering ) const
{
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
  auto& systemsPool = rendering == true
                    ? mRenderSystemsEnabled
                    : mSystemsEnabled;

  systemsPool[systemId] = mSystemsPool[systemId];
}

void
SystemManager::disable(
  const SystemId& systemId )
{
  mSystemsEnabled.erase(systemId);
  mRenderSystemsEnabled.erase(systemId);
}

void
SystemManager::Register(
  const SystemId&   systemId,
  const SystemFunc& system )
{
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
    result.insert(systemId);

  return result;
}

} // namespace cqde::types
