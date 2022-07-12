#include <cqde/types/CallbackManager.hpp>


namespace cqde::types
{

void
CallbackManager::Execute(
  const cqde::CallbackId& callbackId,
  entt::registry& registry,
  const std::vector <std::any>& args ) const
{
  mCallbacks.at(callbackId)(registry, args);
}

void
CallbackManager::Register(
  const cqde::CallbackId& callbackId,
  const CallbackFunc&     callback )
{
  mCallbacks[callbackId] = callback;
}

} // namespace cqde::types
