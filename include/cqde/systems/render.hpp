#pragma once

#include <entt/fwd.hpp>


namespace cqde::systems
{

void RenderBufferClearSystem( entt::registry& );

void EditorRenderSystem( entt::registry& );

void RenderSystem( entt::registry& );

void LightingSystem( entt::registry& );

void EditorEntityHighlightSystem( const entt::registry& );

void InteractionHighlightSystem( const entt::registry& );

} // namespace cqde::systems
