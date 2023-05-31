#pragma once

#include <cqde/types/graphics/GlVertexArray.hpp>


namespace cqde::types
{

struct MeshStorage
{
  GlVertexArray vao {};

  GlBuffer verticesBuffer {};
  GlBuffer texCoordsBuffer {};
  GlBuffer elementBuffer {};

  size_t elementCount {};
  size_t vertexCount {};


  MeshStorage();
  ~MeshStorage();

  void resize(
    const size_t vertices,
    const size_t elements );

  void resizeBuffer(
    GlBuffer&,
    const size_t attribIndex,
    const size_t bindingIndex,
    const size_t bufferSize,
    const size_t stride );
};

} // namespace cqde::types
