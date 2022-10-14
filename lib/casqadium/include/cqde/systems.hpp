#pragma once

#include <entt/fwd.hpp>


namespace cqde::systems
{

void EditorSystem( entt::registry& );

void EditorCullingSystem( entt::registry& );

void EditorEntityHighlightSystem( entt::registry& );

void MouseCenteringSystem( entt::registry& );

void MouseHidingSystem( entt::registry& );

void CullingSystem( entt::registry& );

void RenderSystem( entt::registry& );

void RenderBufferClearSystem( entt::registry& );

void LightingSystem( entt::registry& );

void SequenceSystem( entt::registry& );

} // namespace cqde::systems
