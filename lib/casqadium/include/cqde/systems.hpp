#pragma once

#include <entt/fwd.hpp>


namespace cqde::systems
{

void EditorSystem( entt::registry& registry );

void EditorEntityHighlightSystem( entt::registry& registry );

void MouseCenteringSystem( entt::registry& registry );

void MouseHidingSystem( entt::registry& registry );

void CullingSystem( entt::registry& registry );

void RenderSystem( entt::registry& registry );

void RenderBufferClearSystem( entt::registry& registry );

void LightingSystem( entt::registry& registry );

void SequenceSystem( entt::registry& registry );

} // namespace cqde::systems
