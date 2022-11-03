#pragma once

#include <entt/fwd.hpp>


namespace cqde::systems
{

void EditorSystem( entt::registry& );

void EditorCullingSystem( entt::registry& );

void EditorEntityHighlightSystem( const entt::registry& );

void EditorPhysicsDebugRenderSystem( const entt::registry& );

void MouseCenteringSystem( const entt::registry& );

void MouseHidingSystem( const entt::registry& );

void CullingSystem( entt::registry& );

void RenderSystem( entt::registry& );

void RenderBufferClearSystem( entt::registry& );

void LightingSystem( entt::registry& );

void SequenceSystem( entt::registry& );

void PhysicsSystem( entt::registry& );

void PhysicsDebugRenderSystem( const entt::registry& );

} // namespace cqde::systems
