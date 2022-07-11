#include <cqde/types/VertexBuffer.hpp>


namespace cqde::types
{

bool
VertexBuffer::operator > ( const VertexBuffer& other ) const
{
  return depth > other.depth;
}

VertexBuffer::WindingOrder
VertexBuffer::windingOrderUpdate(
  const bool yAxisUp )
{
  float area = {};

  for ( size_t i = 0, iNext = 1;
        i < vertices.size();
        ++i, iNext = (i + 1) % vertices.size() )
    area += (vertices[iNext].x - vertices[i].x)
          * (vertices[iNext].y + vertices[i].y);

  return WindingOrder(yAxisUp ? area > 0.0f : area < 0.0f);
}

} // namespace cqde::types
