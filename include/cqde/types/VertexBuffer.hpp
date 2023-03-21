#pragma once

#include <cqde/types/graphics/GlVertexArray.hpp>

#include <glm/vec3.hpp>

#include <vector>


namespace cqde::types
{

struct VertexBuffer
{
  std::vector <glm::vec3> vertices {};
  std::vector <uint32_t> indices {};

  GlVertexArray vao {};

  GlBuffer vbo {};
  GlBuffer ebo {};


  VertexBuffer() = default;
  VertexBuffer( VertexBuffer&& );
  VertexBuffer& operator = ( VertexBuffer&& );

  void create();
  void destroy();
};

struct VertexBufferComparator
{
  bool operator () ( const VertexBuffer&, const VertexBuffer& ) const;
};

} // namespace cqde::types
