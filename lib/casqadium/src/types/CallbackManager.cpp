#include <cqde/types/CallbackManager.hpp>

#include <cqde/common.hpp>
#include <cqde/util/logger.hpp>


namespace cqde::types
{

void
CallbackManager::execute(
  const CallbackId& callbackId,
  entt::registry&   registry,
  const std::vector <std::any>& args ) const
{
  if ( mCallbacks.count(callbackId) > 0 )
    return mCallbacks.at(callbackId)(registry, args);

  LOG_ERROR("Can't execute unknown callback '{}'", callbackId.str());
  CQDE_ASSERT_DEBUG(false, return);
}

void
CallbackManager::executeLater(
  const CallbackFunc& callback,
  const CallbackArgs& args )
{
  mDelayedCallbacks.push_back({callback, args});
}

void
CallbackManager::delayedExecution(
  entt::registry& registry )
{
  for ( const auto& [callback, args] : mDelayedCallbacks )
    callback(registry, args);

  mDelayedCallbacks.clear();
}

void
CallbackManager::Register(
  const CallbackId&   callbackId,
  const CallbackFunc& callback )
{
  mCallbacks[callbackId] = callback;
}

std::vector <CallbackId>
CallbackManager::callbacks() const
{
  std::vector <CallbackId> result {};

  for ( const auto& [callbackId, callback] : mCallbacks )
    result.push_back(callbackId);

  return result;
}

} // namespace cqde::types
