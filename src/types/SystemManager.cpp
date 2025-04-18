#include <cqde/types/SystemManager.hpp>

#include <cqde/assert.hpp>
#include <cqde/json_helpers.hpp>

#include <cqde/logger.hpp>

#include <json/value.h>

#include <algorithm>


namespace cqde::types
{


const static Json::Value systemsStateReference =
[]
{
  using ValueType = Json::ValueType;
  using namespace std::string_literals;

  Json::Value reference = ValueType::arrayValue;
  reference.setComment("// systems state root must be a JSON array"s,
                       Json::CommentPlacement::commentBefore);

  reference.append(ValueType::stringValue);
  reference.begin()->setComment("// systems state element must be a JSON string"s,
                                Json::CommentPlacement::commentBefore);

  return reference;
}();

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
         (system.phase & phase) == phase )
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
SystemManager::deactivate()
{
  for ( auto& system : mSystems )
    system.active = false;
}

void
SystemManager::Register(
  const SystemId& systemId,
  const Callback& callback,
  const Phase     phase )
{
  LOG_DEBUG("Registering system '{}'",
            systemId.str());

  auto pos = systemIter(systemId);

  if ( pos != mSystems.end() )
    ++pos;

  mSystems.insert(
    pos,
    {callback, systemId,
    phase, false} );
}

void
SystemManager::RegisterBefore(
  const SystemId& beforeId,
  const SystemId& systemId,
  const Callback& callback,
  const Phase     phase )
{
  LOG_DEBUG("Registering system '{}' before '{}'",
            systemId.str(), beforeId.str());

  const auto pos = systemIter(beforeId);

  if ( pos == mSystems.end() )
    CQDE_ASSERT_DEBUG(false, {});

  mSystems.insert(
    pos,
    {callback, systemId,
    phase, false} );
}

void
SystemManager::RegisterAfter(
  const SystemId& afterId,
  const SystemId& systemId,
  const Callback& callback,
  const Phase     phase )
{
  LOG_DEBUG("Registering system '{}' after '{}'",
            systemId.str(), afterId.str());

  auto pos = systemIter(afterId);

  if ( pos != mSystems.end() )
    pos += std::min(decltype(mSystems)::difference_type{2},
                    std::distance(pos, mSystems.end()));

  else
    CQDE_ASSERT_DEBUG(false, {});

  mSystems.insert(
    pos,
    {callback, systemId,
    phase, false} );
}

void
SystemManager::Validate(
  const Json::Value& systemsJson )
{
  jsonValidateArray(systemsJson, systemsStateReference);
}

void
SystemManager::deserialize(
  const Json::Value& systemsJson )
{
  LOG_DEBUG("Deserializing systems state");

  deactivate();

  for ( const auto& systemId : systemsJson )
    activate(systemId.asString());
}

Json::Value
SystemManager::serialize() const
{
  LOG_DEBUG("Serializing systems state");

  Json::Value result {Json::arrayValue};

  for ( const auto& systemId : systemsActive(Phase::Logic) )
    result.append(systemId.str());

  for ( const auto& systemId : systemsActive(Phase::Render) )
    result.append(systemId.str());

  return result;
}

bool
SystemManager::isActive(
  const SystemId& systemId ) const
{
  for ( const auto& system : mSystems )
    if ( system.id == systemId &&
         system.active == true )
      return true;

  return false;
}

System::Phase
SystemManager::phase(
  const SystemId& systemId )
{
  return systemIter(systemId)->phase;
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
SystemManager::systems(
  const Phase phase ) const
{
  std::vector <SystemId> result {};

  for ( const auto& system : mSystems )
    if ( (system.phase & phase) == phase )
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
         (system.phase & phase) == phase )
      result.push_back(system.id);

  return result;
}

} // namespace cqde::types
