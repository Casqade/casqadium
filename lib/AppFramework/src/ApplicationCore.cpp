#include <TimeUtils/Duration.hpp>

#include <AppFramework/ApplicationCore.hpp>
#include <AppFramework/RenderQueue.hpp>
#include <AppFramework/TickTimestamp.hpp>

#include <chrono>
#include <thread>
#include <future>


ApplicationCore::ApplicationCore( const ApplicationLoop appLoop,
                                  const uint64_t tickRate,
                                  const uint64_t frameRate )
  : mAppLoop(appLoop)
  , mRunning(true)
  , mTickInterval(tickRate > 0 ? 1.0 / tickRate : 0.0)
  , mFrameInterval(frameRate > 0 ? 1.0 / frameRate : 0.0)
  , mTickPrevious(TimeUtils::Now())
  , mFramePrevious(TimeUtils::Now())
{}

int32_t
ApplicationCore::update()
{
  uint32_t ticks = 0;
  if ( mTickInterval != 0 )
  {
    TimeUtils::SleepUntil(mTickPrevious + mTickInterval);
    for ( const auto currentTime = TimeUtils::Now();
          currentTime >= mTickPrevious + mTickInterval;
          mTickPrevious += mTickInterval )
      ++ticks;
  }

  int32_t result = mAppLoop( ticks, mTickInterval );
  RenderQueue::SwapBuffers();

  if ( result >= 0 )
    mRunning = false;

  return result;
}

void
ApplicationCore::render()
{
  while ( mRunning )
  {
    const auto currentTime = TimeUtils::Now();

    if ( mFrameInterval > 0 )
    {
      uint64_t frames = 0;
      TimeUtils::SleepUntil(mFramePrevious + mFrameInterval);
      for ( ;
            currentTime >= mFramePrevious + mFrameInterval;
            mFramePrevious += mFrameInterval )
        ++frames;

      if ( frames == 0 )
        continue;
    }

    auto renderQueue = RenderQueue::Read();
    if ( renderQueue.size() == 0 )
      continue;

    const auto tickTimestamp = std::dynamic_pointer_cast <TickTimestamp> (renderQueue.front());
    const auto interpolation = tickTimestamp ? currentTime - tickTimestamp->latestTickTimestamp() : currentTime - mFramePrevious;
    renderQueue.pop();

    while ( renderQueue.size() > 0 )
    {
        renderQueue.front()->draw(interpolation);
        renderQueue.pop();
    }
  }
}

int32_t
ApplicationCore::Run( const ApplicationLoop appLoop,
                      const uint64_t tickRate,
                      const uint64_t frameRate )
{
  ApplicationCore app( appLoop, tickRate, frameRate );
  std::thread threadRender( std::bind(&ApplicationCore::render, &app) );

  int32_t result = -1;
  while ( result == -1 )
    result = app.update();

  threadRender.join();

  return result;
}
