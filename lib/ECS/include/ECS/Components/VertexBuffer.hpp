#ifndef ECS_COMPONENTS_VERTEXBUFFER_HPP
#define ECS_COMPONENTS_VERTEXBUFFER_HPP

#include <olcPGE/olcPixelGameEngine.hpp>

#include <vector>
#include <string>


namespace ECS
{

namespace Components
{

struct VertexBuffer
{
  std::vector <olc::vf2d> vertices;

  float depth;

  enum class WindingOrder
  {
    CounterClockWise,
    ClockWise,

  } windingOrder;


  bool operator > ( const VertexBuffer& other ) const;
};

VertexBuffer::WindingOrder
GetWindingOrder(  const VertexBuffer& vertices,
                  const bool yAxisUp = false );

} // namespace Components

} // namespace ECS

#endif
