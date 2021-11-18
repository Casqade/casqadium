#ifndef APPLICATION_CORE_HPP
#define APPLICATION_CORE_HPP

#include <TimeUtils/Duration.hpp>

#include <functional>
#include <cstdint>
#include <atomic>


class ApplicationCore
{
  typedef std::function <int32_t (const uint32_t ticks,
                                  const TimeUtils::Duration& deltaTime)> ApplicationLoop;
  ApplicationLoop mAppLoop;

  std::atomic <bool> mRunning;

  TimeUtils::Duration mTickInterval;
  TimeUtils::Duration mFrameInterval;

  TimeUtils::Duration mTickPrevious;
  TimeUtils::Duration mFramePrevious;

  int32_t update();
  void    render();

  ApplicationCore(  const ApplicationLoop,
                    const uint64_t tickRate,
                    const uint64_t frameRate );

public:
  ApplicationCore() = delete;

  static int32_t Run( const ApplicationLoop,
                      const uint64_t tickRate = 30,
                      const uint64_t frameRate = 60 );
};


#endif
