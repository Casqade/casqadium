#pragma once

#include <olcPGE/olcPixelGameEngine.hpp>


namespace olc
{

enum class MouseInputId
{
  ENUM_BEGIN = olc::Key::ENUM_END,
  ButtonLeft = ENUM_BEGIN,
  ButtonRight,
  ButtonMiddle,
  ButtonX1,
  ButtonX2,

  MoveX,
  MoveY,

  Wheel
};

} // namespace olc
