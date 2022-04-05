#ifndef ECS_SYSTEMS_RENDER_SYSTEM_HPP
#define ECS_SYSTEMS_RENDER_SYSTEM_HPP

#include <entt/fwd.hpp>

#include <glm/fwd.hpp>


namespace ECS
{

namespace Components
{
struct GeometryBuffer;
struct VertexBuffer;
}

namespace Systems
{

Components::VertexBuffer vertexShader(
  const Components::GeometryBuffer& cGeometryBuffer,
  const glm::mat4& modelViewMatrix,
  const glm::mat4& projectionMatrix,
  const glm::vec4& viewport );

void CullSystem( entt::registry& registry );

void
RenderSystem( entt::registry& );

} // namespace Systems

} // namespace ECS

#endif
