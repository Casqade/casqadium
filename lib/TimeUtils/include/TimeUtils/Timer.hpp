#ifndef TIMER_HPP
#define TIMER_HPP

#include <TimeUtils/Duration.hpp>


class Timer
{
  using Duration = TimeUtils::Duration;

  bool mCounting;

  Duration mDuration;
  Duration mRemainingTime;

public:
  Timer( const Duration& duration );
  void start();
  void start( const Duration& newDuration );
  void restart();
  void stop();

  void update( const Duration& deltaTime );

  bool isReady() const;
};


#endif
