#pragma once

#include <entt/fwd.hpp>


namespace cqde::systems
{

void MouseGrabSystem( const entt::registry& );

void MouseCursorOverrideSystem( entt::registry& );

} // namespace cqde::systems
