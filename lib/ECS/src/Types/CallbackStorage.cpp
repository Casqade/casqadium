#include <ECS/Types/CallbackStorage.hpp>


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

} // namespace Types

} // namespace ECS
