#pragma once

#include <entt/fwd.hpp>


namespace cqde::systems
{

void RenderBufferClearSystem( entt::registry& );

void CullingSystem( entt::registry& );

void EditorCullingSystem( entt::registry& );

void LightingSystem( entt::registry& );

void RenderSystem( entt::registry& );

void EditorEntityHighlightSystem( const entt::registry& );

} // namespace cqde::systems
