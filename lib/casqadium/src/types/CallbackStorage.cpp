#include <cqde/types/CallbackStorage.hpp>
#include <cqde/components/InputController.hpp>

#include <functional>


namespace cqde::types
{

template <typename CallbackType>
void
CallbackStorage <CallbackType>::Register(
  const cqde::CallbackId  callbackId,
  const CallbackType      callback )
{
  mCallbacks[callbackId] = callback;
}

template class
CallbackStorage <std::function <void(const entt::entity, cqde::compos::InputController&)>>;


} // namespace cqde::types
