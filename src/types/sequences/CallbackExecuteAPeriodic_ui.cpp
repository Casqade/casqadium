#include <cqde/types/sequences/CallbackExecuteAPeriodic.hpp>


namespace cqde::types
{

void
CallbackExecuteAPeriodic::ui_show(
  const entt::registry& registry,
  const entt::entity entity )
{
  RandomDelay::ui_show(registry, entity);
  CallbackExecute::ui_show(registry, entity);
}

} // namespace cqde::types
