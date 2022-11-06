#include <AnotherDayAtHospital.hpp>

#include <GameStateDemo.hpp>

#include <cqde/util/logger.hpp>

#include <olcPGE/olcPGEX_TTF.hpp>

#include <imgui.h>
#include <ImGuizmo.h>



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
  TimeUtils::TimePeriodInit();
}

AnotherDayAtHospital::~AnotherDayAtHospital()
{
  TimeUtils::TimePeriodDeinit();
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

  ImGui::NewFrame();
  ImGuizmo::BeginFrame();

  auto result = mGameStateController.update( ticks, tickInterval );

  ImGui::EndFrame();

  return result;
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

  mGameStateController.setState <GameStateDemo> (mConfigManager);

  return true;
}

bool
AnotherDayAtHospital::OnUserUpdate( float )
{
  static const bool tickRateLimited = mTickInterval > TimeUtils::Duration();
  static const bool frameRateLimited = mFrameInterval > TimeUtils::Duration();

  const auto tickTimeToSleep = mTickPrevious + mTickInterval;
  const auto frameTimeToSleep = mFramePrevious + mFrameInterval;

  TimeUtils::SleepUntil( std::min(tickTimeToSleep, frameTimeToSleep) );


  mEventHandler.update();

  bool running = true;
  static bool worldStateUpdated {};
  uint32_t ticks = 0;

  const auto currentTime = TimeUtils::Now();

  if ( tickRateLimited == true )
    for ( ; currentTime >= mTickPrevious + mTickInterval;
            mTickPrevious += mTickInterval )
      ++ticks;
  else
  {
    ticks = 1;
    mTickInterval = currentTime - mTickPrevious;
    mTickPrevious = currentTime;
  }

  mImGui.ImGui_ImplPGE_NewFrame();

  if ( ticks != 0 )
  {
    running = update( ticks, mTickInterval );
    worldStateUpdated = true;
  }

  if ( running == false )
    return running;

  uint32_t frames = 0;

  if ( frameRateLimited == true )
    for ( ; currentTime >= mFramePrevious + mFrameInterval;
            mFramePrevious += mFrameInterval )
      ++frames;
  else
    frames = 1;

  if ( frames != 0 &&
       (frameRateLimited == false || worldStateUpdated == true) )
  {
    NewFrame();
    mGameStateController.render(frames, mFrameInterval);
    worldStateUpdated = false;
  }

  return true;
}
