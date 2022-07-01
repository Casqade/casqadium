#include <cqde/types/input/InputCallbackStorage.hpp>


namespace cqde::types
{

void
InputCallbackStorage::Execute(
  const cqde::CallbackId callbackId,
  const entt::entity entity,
  cqde::compos::InputController& controller )
{
  mCallbacks[callbackId](entity, controller);
}

} // namespace cqde::types
