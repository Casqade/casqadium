#include <AnotherDayAtHospital.hpp>

#include <GameStateForeword.hpp>
#include <Variables.hpp>
#include <Logger.hpp>

#include <forward_list>



std::string backStoryText {"Episode IV;\
A NEW HOPE;\
It is a period of civil war.;\
Rebel spaceships, striking;\
from a hidden base, have won;\
their first victory against;\
the evil Galactic Empire.;\
;\
During the battle, Rebel;\
spies managed to steal secret;\
plans to the Empire's;\
ultimate weapon, the DEATH;\
STAR, an armored space;\
station with enough power to;\
destroy an entire planet.;\
;\
Pursued by the Empire's;\
sinister agents, Princess;\
Leia races home aboard her;\
starship, custodian of the;\
stolen plans that can save;\
her people and restore;\
freedom to the galaxy...."};


std::vector <std::string> TitleTextEntries;
std::vector <std::string> BackStoryTextEntries;


AnotherDayAtHospital::AnotherDayAtHospital(
  const uint64_t tickRate,
  const uint64_t frameRate )
  : olc::PixelGameEngine()
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
AnotherDayAtHospital::loadResources()
{
  std::forward_list <std::pair <StringId, std::string>> strings =
  {
    { StringId::Foreword, "A long time ago in a galaxy far,\nfar away..." },
    { StringId::Title, "Another Day;At Hospital" },
    { StringId::Backstory, backStoryText },
    { StringId::ForewordInputPrompt, "Hold any key" },
  };

  std::forward_list <std::pair <FontId, std::string>> fonts =
  {
//    { FontId::Munro, "munro.png" },
    { FontId::Jetbrains, "jetbrains.png" },
    { FontId::FranklinGothic, "framd.png" },
    { FontId::FranklinGothicItalic, "framdit.png" },
  };

  std::forward_list <std::pair <SpriteId, std::string>> sprites =
  {
    { SpriteId::WindowIcon, "window_icon.png" },
  };

  std::forward_list <std::pair <SoundId, std::string>> sounds =
  {
//    { SoundId::Null, "" },
  };

  std::forward_list <std::pair <MusicId, std::string>> musics =
  {
    { MusicId::TitleTheme, "title.ogg" },
  };

  for ( const auto& string : strings )
    Strings::Load( string.first, string.second );

  for ( const auto& font : fonts )
    if ( Fonts::Load( font.first, font.second ) == false )
    {
      Log << "ERROR: Failed to load font " << font.second;
      return false;
    }

  for ( const auto& sprite : sprites )
    if ( Sprites::Load( sprite.first, sprite.second ) == false )
    {
      Log << "ERROR: Failed to load sprite " << sprite.second;
      return false;
    }

  for ( const auto& sound : sounds )
    if ( Sounds::Load( sound.first, sound.second ) == false )
    {
      Log << "ERROR: Failed to load sound " << sound.second;
      return false;
    }

  for ( const auto& music : musics )
    if ( Music::Load( music.first, music.second ) == false )
    {
      Log << "ERROR: Failed to load music " << music.second;
      return false;
    }

  std::string entry {};
  std::vector <std::string> lines {};

  std::istringstream titleText ( Strings::Get( StringId::Title ));
  while ( std::getline( titleText, entry, ';' ) )
    TitleTextEntries.push_back( entry );

  std::istringstream backStoryText( Strings::Get( StringId::Backstory ) );
  while ( std::getline( backStoryText, entry, ';' ) )
    BackStoryTextEntries.push_back( entry );

  return true;
}

bool
AnotherDayAtHospital::update(
  const uint32_t ticks,
  const TimeUtils::Duration tickInterval )
{
  mEventHandler.update();

  olc::Event event;
  while ( mEventHandler.pollEvent(event) )
    mGameStateController.handleEvent( event );

  return mGameStateController.update( ticks, tickInterval );
}

bool
AnotherDayAtHospital::OnUserCreate()
{
//  SetCursor(NULL);

  if ( loadResources() == false )
    return false;

  mGameStateController.setState <GameStateForeword> ();

  return true;
}

bool
AnotherDayAtHospital::OnUserUpdate( float )
{
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
    mGameStateController.render();

  TimeUtils::SleepUntil( std::min( mTickPrevious + (tickRateLimited ? mTickInterval : TimeUtils::Duration()),
                                   mFramePrevious + mFrameInterval ) );

  return true;
}
