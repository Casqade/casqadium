#include <TimeUtils/Timer.hpp>


Timer::Timer( const Duration& duration )
  : mCounting(false)
  , mDuration(duration)
  , mRemainingTime(duration)
{}

void
Timer::start()
{
  mCounting = true;
}

void
Timer::start( const Duration& newDuration )
{
  mDuration = mRemainingTime = newDuration;

  start();
}

void
Timer::restart()
{
  start( mDuration );
}

void
Timer::stop()
{
  mCounting = false;
}

void
Timer::update( const Duration& deltaTime )
{
  if ( isReady() )
    return;

  if ( mRemainingTime > Duration(0.0) )
    mRemainingTime -= deltaTime;
  else
  {
    mRemainingTime = Duration(0.0);
    stop();
  }
}

bool
Timer::isReady() const
{
  return mRemainingTime <= Duration(0.0);
}
