#pragma once

#include <entt/fwd.hpp>


namespace cqde::systems
{

void PhysicsSystem( entt::registry& );

void PhysicsDebugRenderSystem( entt::registry& );

void EditorPhysicsDebugRenderSystem( entt::registry& );

} // namespace cqde::systems
