#pragma once

#include <cstddef>


namespace cqde::types
{

struct Mesh
{
  size_t firstElementIndex {};
  size_t elementCount {};

  size_t firstVertexIndex {};
  size_t vertexCount {};
};

} // namespace cqde::types
