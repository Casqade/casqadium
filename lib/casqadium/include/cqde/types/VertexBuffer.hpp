#pragma once

#include <olcPGE/olcPixelGameEngine.hpp>

#include <vector>


namespace cqde::types
{

struct VertexBuffer
{
  std::vector <olc::vf2d> vertices {};

  float depth {};

  enum class WindingOrder
  {
    CounterClockWise,
    ClockWise,

  } windingOrder {};


  VertexBuffer() = default;
};

struct VertexBufferComparator
{
  bool operator () ( const VertexBuffer&, const VertexBuffer& ) const;
};

} // namespace cqde::types
