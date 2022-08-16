#pragma once

#include <cqde/fwd.hpp>

#include <entt/fwd.hpp>

#include <glm/fwd.hpp>

#include <vector>


namespace cqde::systems
{

types::VertexBuffer
vertexShader(
  const std::vector <glm::vec3>& vertices,
  const glm::mat4& modelViewMatrix,
  const glm::mat4& projectionMatrix,
  const glm::vec4& viewport );

void CullingSystem( entt::registry& registry );

} // namespace cqde::systems
