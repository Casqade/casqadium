#include <ECS/Types/CallbackStorage.hpp>
#include <ECS/Components/InputController.hpp>

#include <functional>


namespace ECS
{

namespace Types
{

template <typename CallbackType>
void
CallbackStorage <CallbackType>::Register(
  const ECS::CallbackId callbackId,
  const CallbackType    callback )
{
  mCallbacks[callbackId] = callback;
}

template class
CallbackStorage <std::function <void(const entt::entity, ECS::Components::InputController&)>>;


} // namespace Types

} // namespace ECS
