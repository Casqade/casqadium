#pragma once

#include <vector>
#include <cstddef>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>


namespace cqde::types
{

struct VertexBuffer
{
  std::vector <glm::vec3> vertices {};
  std::vector <glm::vec2> uvCoordinates {};
  std::vector <uint32_t> indices {};
};

} // namespace cqde::types
