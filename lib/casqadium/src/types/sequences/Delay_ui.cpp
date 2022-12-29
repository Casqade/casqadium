#include <cqde/types/sequences/Delay.hpp>

#include <imgui.h>


namespace cqde::types
{

void
Delay::ui_show(
  const entt::registry&,
  const entt::entity )
{
  mTime.ui_show();
}

} // namespace cqde::types
