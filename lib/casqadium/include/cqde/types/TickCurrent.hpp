#pragma once

#include <TimeUtils/Duration.hpp>


namespace cqde::types
{

struct TickCurrent
{
  uint32_t ticksElapsed {};

  TimeUtils::Duration tickInterval {};
  TimeUtils::Duration lastTickTimepoint {TimeUtils::Now()};


  TickCurrent() = default;
};

} // namespace cqde::types
