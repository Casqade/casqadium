#pragma once

#include <olcPGE/olcPixelGameEngine.hpp>

#include <vector>


namespace ECS
{

namespace Types
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

} // namespace Types


} // namespace ECS
