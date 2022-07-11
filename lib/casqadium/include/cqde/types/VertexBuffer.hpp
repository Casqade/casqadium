#pragma once

#include <olcPGE/olcPixelGameEngine.hpp>

#include <vector>


namespace cqde::types
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


  VertexBuffer() = default;


  bool operator > ( const VertexBuffer& other ) const;

  void windingOrderUpdate( const bool yAxisUp = false );
};

} // namespace cqde::types
