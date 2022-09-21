#include <AnotherDayAtHospital.hpp>

#include <GameStateEcsSandbox.hpp>

#include <olcPGE/olcPGEX_TTF.hpp>

#include <cqde/util/logger.hpp>


AnotherDayAtHospital::AnotherDayAtHospital(
  const ConfigManager& configManager )
  : mConfigManager{configManager}
  , mTickInterval(configManager.tickRate() > 0
                  ? 1.0 / configManager.tickRate()
                  : TimeUtils::Duration())
  , mFrameInterval(configManager.frameRate() > 0
                   ? 1.0 / configManager.frameRate()
                   : TimeUtils::Duration())
{
  sAppName = u8"Another Day At Hospital";
}

AnotherDayAtHospital::~AnotherDayAtHospital()
{
  mGameStateController.clearState();

  if ( olc::Font::deinit() != olc::rcode::OK )
    LOG_ERROR("Failed to deinitialize olc::Font: {}",
              olc::Font::GetLibraryErrorMessage());
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
  if ( olc::Font::init() != olc::rcode::OK )
  {
    LOG_ERROR("Failed to initialize olc::Font: {}",
              olc::Font::GetLibraryErrorMessage());
    return false;
  }

  mGameLayer = CreateLayer();

  EnableLayer(mGameLayer, true);
  SetDrawTarget(mGameLayer);

  mGameStateController.setState <GameStateEcsSandbox> (mConfigManager);

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
    SetDrawTarget(mGameLayer); // Dear ImGui compatibility
    mGameStateController.render(frames, mFrameInterval);
  }

  const auto tickTimeToSleep = mTickPrevious + mTickInterval;
  const auto frameTimeToSleep = mFramePrevious + mFrameInterval;

  TimeUtils::SleepUntil( std::min(tickTimeToSleep, frameTimeToSleep) );

  return true;
}
