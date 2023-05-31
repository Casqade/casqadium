#pragma once

#include <entt/fwd.hpp>


namespace cqde::systems
{

void RenderBufferClearSystem( entt::registry& );

void EditorRenderSystem( entt::registry& );

void RenderSystem( entt::registry& );

void EditorEntityHighlightSystem( entt::registry& );

void InteractionHighlightSystem( entt::registry& );

} // namespace cqde::systems
