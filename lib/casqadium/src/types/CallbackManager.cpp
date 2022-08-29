#include <cqde/types/CallbackManager.hpp>


namespace cqde::types
{

void
CallbackManager::execute(
  const CallbackId& callbackId,
  entt::registry&   registry,
  const std::vector <std::any>& args ) const
{
  mCallbacks.at(callbackId)(registry, args);
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
