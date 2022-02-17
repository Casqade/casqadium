#include <SFML/Audio/Music.hpp>

#include <GameStateTitle.hpp>
#include <Logger.hpp>
#include <Variables.hpp>

#include <TimeUtils/Duration.hpp>


extern std::vector <std::string> BackStoryTextEntries;
extern std::vector <std::string> TitleTextEntries;


GameStateTitle::GameStateTitle( GameStateController* const stateController )
  : GameState(stateController)
  , mState(StateLocal::Title)
  , mTitlePos({0.0f, 0.0f, 0.75f})
  , mBackstoryPos({ 0.0f, 150.0f, 0.0f })
  , mPressedKeys()
{}

void
GameStateTitle::updateTitle( const TimeUtils::Duration dt )
{
  if ( mPressedKeys.size() > 0 )
    mTitlePos.z += 0.5 * (double) dt;

  for ( auto it = TitleTextEntries.begin();
        it != TitleTextEntries.end();
        ++it )
  {
    auto& line = *it;
    const auto lineIndex = it - TitleTextEntries.begin();

//    move text away
  }
}

void
GameStateTitle::updateBackStory( const TimeUtils::Duration dt )
{
  if ( mPressedKeys.size() > 0 )
    mBackstoryPos.z += 0.5 * (double) dt;

  for ( auto it = BackStoryTextEntries.begin();
        it != BackStoryTextEntries.end();
        ++it )
  {
    auto& line = *it;
    const auto lineIndex = it - BackStoryTextEntries.begin();

//    move text up
  }
}

bool
GameStateTitle::update( const uint32_t ticks,
                        const TimeUtils::Duration tickInterval )
{
  const double dt = (double) tickInterval;

  static auto titleDelay = TimeUtils::Duration(2.0);
  if ( (titleDelay -= tickInterval) > TimeUtils::Duration() )
    return true;

  updateTitle( dt );

  static auto backStoryDelay = TimeUtils::Duration(10.0);
  if ( (backStoryDelay -= tickInterval) > TimeUtils::Duration() )
    return true;

  updateBackStory( dt );

  return true;
}

void
GameStateTitle::keyEvent( const olc::Event event )
{
  const olc::Event::KeyEvent key = event.key;

  switch (event.type)
  {
    case olc::Event::EventType::KeyPressed:
    {
      mPressedKeys.emplace(key.code);
      static sf::Music& music = Music::Get(MusicId::TitleTheme);
      if (music.getStatus() != sf::Music::Playing)
          music.play();

      return;
    }

    case olc::Event::EventType::KeyReleased:
    {
      auto pressedKey = mPressedKeys.find(key.code);
      if ( pressedKey != mPressedKeys.end() )
        mPressedKeys.erase( pressedKey );

      if ( mPressedKeys.size() > 0 )
        return;

      Music::Get(MusicId::TitleTheme).pause();

      return;
    }

    default:
      return;
  }
}

void
GameStateTitle::render()
{
  for ( auto& text : TitleTextEntries )
    mPGE->DrawStringDecal( {}, text );

  for ( auto& text : BackStoryTextEntries )
    mPGE->DrawStringDecal( {}, text );
}
