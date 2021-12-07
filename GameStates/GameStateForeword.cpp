#include <SFML/Audio/SoundBuffer.hpp>
#include <SFML/Audio/Music.hpp>

#include <GameStateForeword.hpp>

#include <TimeUtils/Duration.hpp>

#include <Logger.hpp>
#include <Variables.hpp>
#include <GameStateController.hpp>
#include <GameStateTitle.hpp>


GameStateForeword::GameStateForeword( GameStateController* const stateController )
  : GameState(stateController)
  , mState(StateLocal::TransitingIn)
//  , mForeword(Strings::Get(StringId::Foreword),
//              Fonts::Get(FontId::FranklinGothic))
  , mInputPrompt(Strings::Get(StringId::ForewordInputPrompt), Fonts::Get(FontId::FranklinGothic),
                 0.2, 0.2)
  , mFade(  { 0,
              0 },
            {olc::BLACK, olc::BLANK},
            0.75 )
  , mInputPromptDelay()
  , mTransitingOutTimer()
  , mPressedKeys()
{
//  mForeword.setFillColor( sf::Color( 37, 203, 232 ) );

//  mForeword.setOrigin( mForeword.getLocalBounds().width * 0.5f,
//                       mForeword.getLocalBounds().height * 0.5f );

//  mForeword.setPosition( window.getSize().x * 0.5f,
//                         window.getSize().y * 0.5f );


//  mInputPrompt.setFillColor( sf::Color( 37, 203, 232, 0 ) );

//  mInputPrompt.setOrigin( mInputPrompt.getLocalBounds().width * 0.5f,
//                          mInputPrompt.getLocalBounds().height * 0.5f );

//  mInputPrompt.setPosition( window.getSize().x * 0.5f,
//                            window.getSize().y * 0.75f );
}

bool
GameStateForeword::update(  const uint32_t ticks,
                            const TimeUtils::Duration elapsed )
{
  switch ( mState )
  {
    case StateLocal::TransitingIn:
      return transitingIn( elapsed ), true;

    case StateLocal::WaitAccept:
      return waitAccept( elapsed ), true;

    case StateLocal::TransitingOut:
      return transitingOut( elapsed ), true;

    default:
      return true;
  }

  return true;
}

void
GameStateForeword::transitingIn( const TimeUtils::Duration dt )
{
  mFade.update( dt );

  if ( mFade.finished() == false )
    return;

  mState = StateLocal::WaitAccept;
  Log << "WaitAccept";
}

void
GameStateForeword::transitingOut( const TimeUtils::Duration dt )
{
  mInputPrompt.update( dt );

  if ( mPressedKeys.size() == 0 )
    return;

  mFade.update( dt );
  if ( mFade.finished() == false )
    return;

//  static sf::Music& music = Music::Get(MusicId::TitleTheme);
//  if ( music.getStatus() != sf::Music::Status::Playing )
//    music.play();

  Log << "Finished";
  mGameStateController->setState <GameStateTitle> ();
}

void
GameStateForeword::waitAccept( const TimeUtils::Duration dt )
{
  const static sf::Time inputPromptDelay { sf::seconds(2.0f) };

  if ( (mInputPromptDelay += dt) < inputPromptDelay.asSeconds() )
    return;

  if ( mPressedKeys.size() == 0
       && mInputPrompt.isActive() == false )
    mInputPrompt.setState( InputPrompt::State::Shown );

  mInputPrompt.update( dt );
}

void
GameStateForeword::keyEvent( const olc::Event event )
{
  const olc::Event::KeyEvent key = event.key;

  switch (event.type)
  {
    case olc::Event::EventType::KeyPressed:
      switch (mState)
      {
        case StateLocal::WaitAccept:
          mPressedKeys.emplace( key.code );

          mInputPrompt.setState( InputPrompt::State::Inactive );

          mFade.setFadeRange({ olc::BLANK, olc::BLACK });
          mFade.setFadeDuration( 0.75 );

          mState = StateLocal::TransitingOut;
          Log << "TransitingOut";

          return;

        case StateLocal::TransitingOut:
          mPressedKeys.emplace( key.code );
          mInputPrompt.setState( InputPrompt::State::Inactive );
          return;

        default:
          return;
      }
    case olc::Event::EventType::KeyReleased:
      if ( mState == StateLocal::TransitingOut )
      {
        auto pressedKey = mPressedKeys.find(key.code);
        if ( pressedKey != mPressedKeys.end() )
          mPressedKeys.erase( pressedKey );

        if ( mPressedKeys.size() > 0 )
          return;

        mInputPrompt.setState( InputPrompt::State::Shown );
        Music::Get(MusicId::TitleTheme).pause();

      }
    default:
      return;
  }
}

void
GameStateForeword::render( olc::PixelGameEngine* const engine )
{
//  window.draw( mForeword );
//  window.draw( mFade );
//  window.draw( mInputPrompt );
}
