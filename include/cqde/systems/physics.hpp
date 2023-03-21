#pragma once

#include <entt/fwd.hpp>


namespace cqde::systems
{

void PhysicsSystem( entt::registry& );

void PhysicsDebugRenderSystem( const entt::registry& );

void EditorPhysicsDebugRenderSystem( const entt::registry& );

} // namespace cqde::systems
