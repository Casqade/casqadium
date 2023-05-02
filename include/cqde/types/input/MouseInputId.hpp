#pragma once

#include <cqde/alias.hpp>


#include <GLFW/glfw3.h>


namespace cqde::types
{

enum class MouseInputId : InputHwCode
{
  ENUM_START = GLFW_KEY_LAST + 1,

  ButtonLeft = ENUM_START,
  ButtonRight,
  ButtonMiddle,
  ButtonX1,
  ButtonX2,
  ButtonX3,
  ButtonX4,
  ButtonX5,

  MoveX,
  MoveY,
  PosX,
  PosY,

  WheelX,
  WheelY,

  ENUM_END,
};

} // namespace cqde::types
