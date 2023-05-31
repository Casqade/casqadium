#pragma once

#include <cqde/alias.hpp>

#include <TimeUtils/Duration.hpp>


namespace cqde::types
{

struct InputEvent
{
  TimeUtils::Duration tp {TimeUtils::Now()};

  InputHwId inputId {};
  float     amount {};


  InputEvent() = default;
};

} // namespace cqde::types
