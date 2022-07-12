#pragma once

#include <TimeUtils/Duration.hpp>


namespace cqde::types
{

struct FrameCurrent
{
  uint32_t framesElapsed {};

  TimeUtils::Duration frameInterval {};
  TimeUtils::Duration lastFrameTimepoint {TimeUtils::Now()};


  FrameCurrent() = default;
};

} // namespace cqde::types
