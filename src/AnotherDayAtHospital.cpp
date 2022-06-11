#include <AnotherDayAtHospital.hpp>

#include <GameStateEcsSandbox.hpp>


AnotherDayAtHospital::AnotherDayAtHospital(
  const uint64_t tickRate,
  const uint64_t frameRate )
  : olc::PixelGameEngine()
  , mImGui(true)
  , mTickInterval(tickRate > 0 ? 1.0 / tickRate : TimeUtils::Duration())
  , mFrameInterval(frameRate > 0 ? 1.0 / frameRate : TimeUtils::Duration())
  , mTickPrevious(TimeUtils::Now())
  , mFramePrevious(TimeUtils::Now())
  , mGameStateController()
  , mEventHandler(this)
{
  sAppName = u8"Another Day At Hospital";
}

bool
AnotherDayAtHospital::update(
  const uint32_t ticks,
  const TimeUtils::Duration tickInterval )
{
  olc::Event event;
  while ( mEventHandler.pollEvent(event) )
    mGameStateController.handleEvent( event );

  return mGameStateController.update( ticks, tickInterval );
}

bool
AnotherDayAtHospital::OnUserCreate()
{
//  SetCursor(NULL);

  const uint32_t mainLayer = CreateLayer();
  EnableLayer( mainLayer, true );
  SetDrawTarget( mainLayer );

  mGameStateController.setState <GameStateEcsSandbox> ();

  return true;
}

bool
AnotherDayAtHospital::OnUserUpdate( float )
{
  mEventHandler.update();

  static const bool tickRateLimited = mTickInterval > TimeUtils::Duration();
  static const bool frameRateLimited = mFrameInterval > TimeUtils::Duration();

  const auto currentTime = TimeUtils::Now();

  bool running = true;
  uint32_t ticks = 0;

  if ( tickRateLimited )
    for ( ;
          currentTime >= mTickPrevious + mTickInterval;
          mTickPrevious += mTickInterval )
      ++ticks;
  else
  {
    ticks = 1;
    mTickInterval = currentTime - mTickPrevious;
    mTickPrevious = currentTime;
  }

  if ( ticks != 0 )
    running = update( ticks, mTickInterval );

  if ( running == false )
    return running;

  uint32_t frames = 0;

  if ( frameRateLimited )
    for ( ;
          currentTime >= mFramePrevious + mFrameInterval;
          mFramePrevious += mFrameInterval )
      ++frames;
  else
    frames = 1;

  if ( frames != 0 )
  {
    SetDrawTarget(1); // Dead ImGui compatibility
    mGameStateController.render();
  }

  TimeUtils::SleepUntil( mFramePrevious + mFrameInterval );

  return true;
}
