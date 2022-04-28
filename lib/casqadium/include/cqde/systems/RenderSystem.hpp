#pragma once

#include <entt/fwd.hpp>

#include <glm/fwd.hpp>


namespace cqde
{

namespace compos
{
struct GeometryBuffer;
}

namespace types
{
struct VertexBuffer;
}

namespace systems
{

void
RenderSystem( entt::registry& );

} // namespace systems

} // namespace cqde
