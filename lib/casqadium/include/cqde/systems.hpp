#pragma once

#include <entt/fwd.hpp>


namespace cqde::systems
{

void EditorSystem( entt::registry& registry );

void EditorEntityHighlightSystem( entt::registry& registry );

void CullingSystem( entt::registry& registry );

void RenderSystem( entt::registry& registry );

} // namespace cqde::systems
