#include <cqde/types/sequences/CallbackExecutePeriodic.hpp>


namespace cqde::types
{

void
CallbackExecutePeriodic::ui_show(
  const entt::registry& registry,
  const entt::entity entity )
{
  Delay::ui_show(registry, entity);
  CallbackExecute::ui_show(registry, entity);
}

} // namespace cqde::types
