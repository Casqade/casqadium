#include <GameStateForeword.hpp>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Audio/SoundBuffer.hpp>
#include <SFML/Audio/Music.hpp>

#include <TimeUtils/Duration.hpp>
#include <AppFramework/RenderCommand.hpp>
#include <AppFramework/RenderQueue.hpp>

#include <Logger.hpp>
#include <Variables.hpp>
#include <GameStateController.hpp>
#include <GameStateTitle.hpp>


GameStateForeword::GameStateForeword()
  : mState(StateLocal::TransitingIn)
  , mForeword(Strings::Get(StringId::Foreword),
              Fonts::Get(FontId::FranklinGothic))
  , mInputPrompt({Strings::Get(StringId::ForewordInputPrompt), Fonts::Get(FontId::FranklinGothic)},
                 sf::seconds(0.2f), sf::seconds(0.2f))
  , mFade(  sf::Vector2f( window.getSize().x,
                          window.getSize().y ),
            {sf::Color::Black, sf::Color::Transparent},
            sf::seconds(0.75f) )
  , mInputPromptDelay()
  , mTransitingOutTimer()
  , mPressedKeys()
{
  mForeword.setFillColor( sf::Color( 37, 203, 232 ) );

  mForeword.setOrigin( mForeword.getLocalBounds().width * 0.5f,
                       mForeword.getLocalBounds().height * 0.5f );

  mForeword.setPosition( window.getSize().x * 0.5f,
                         window.getSize().y * 0.5f );


  mInputPrompt.setFillColor( sf::Color( 37, 203, 232, 0 ) );

  mInputPrompt.setOrigin( mInputPrompt.getLocalBounds().width * 0.5f,
                          mInputPrompt.getLocalBounds().height * 0.5f );

  mInputPrompt.setPosition( window.getSize().x * 0.5f,
                            window.getSize().y * 0.75f );
}

void
GameStateForeword::update( const sf::Time elapsed )
{
  const float dt = elapsed.asSeconds();

  switch ( mState )
  {
    case StateLocal::TransitingIn:
      return transitingIn( dt );

    case StateLocal::WaitAccept:
      return waitAccept( dt );

    case StateLocal::TransitingOut:
      return transitingOut( dt );

    default:
      return;
  }
}

void
GameStateForeword::transitingIn( const float dt )
{
  mFade.update( dt );

  if ( mFade.finished() == false )
    return;

  mState = StateLocal::WaitAccept;
  Log << "WaitAccept";
}

void
GameStateForeword::transitingOut( const float dt )
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
  GameStateController::setState <GameStateTitle> ();
}

void
GameStateForeword::waitAccept( const float dt )
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
GameStateForeword::keyEvent( const sf::Event event )
{
  const sf::Event::KeyEvent key = event.key;

  switch (event.type)
  {
    case sf::Event::EventType::KeyPressed:
      switch (mState)
      {
        case StateLocal::WaitAccept:
          mPressedKeys.emplace( key.code );

          mInputPrompt.setState( InputPrompt::State::Inactive );

          mFade.setFadeRange({ mFade.getFillColor(), sf::Color::Black });
          mFade.setFadeDuration( sf::seconds(0.75f) );

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
    case sf::Event::EventType::KeyReleased:
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
GameStateForeword::render( sf::RenderWindow& )
{
  RenderQueue::Current().push( std::make_shared <RenderCommand> ( [=] ( const TimeUtils::Duration )
  {
      window.draw( mForeword );
      window.draw( mFade );
      window.draw( mInputPrompt );
  } ) );
}
