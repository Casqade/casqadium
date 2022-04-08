#include <ECS/Types/VertexBuffer.hpp>


namespace ECS
{

namespace Types
{

bool
VertexBuffer::operator > ( const VertexBuffer& other ) const
{
  return depth > other.depth;
}

VertexBuffer::WindingOrder
GetWindingOrder(  const VertexBuffer& vertexBuffer,
                  const bool yAxisUp )
{
  auto vertices = vertexBuffer.vertices;
  float area = {};

  for ( size_t i = 0, iNext = 1;
        i < vertices.size();
        ++i, iNext = (i + 1) % vertices.size() )
    area += (vertices[iNext].x - vertices[i].x)
          * (vertices[iNext].y + vertices[i].y);

  return ECS::Types::VertexBuffer::WindingOrder(yAxisUp ? area > 0.0f : area < 0.0f);
}

} // namespace Components

} // namespace ECS

