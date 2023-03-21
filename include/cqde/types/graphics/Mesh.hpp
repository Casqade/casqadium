#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <vector>


namespace cqde::types
{


struct Mesh
{
  std::vector <glm::vec3> vertices {};
  std::vector <glm::vec3> normals {};
  std::vector <glm::vec2> texCoords {};

  std::vector <uint32_t> indices {};

  uint32_t vao {};
  uint32_t ebo {};
  uint32_t vboVertices {};
  uint32_t vboNormals {};
  uint32_t vboTexCoords {};


  Mesh() = default;

  void create();
};

} // namespace cqde::types
