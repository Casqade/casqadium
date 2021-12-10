#include <GameStateSandbox.hpp>

#include <TimeUtils/Duration.hpp>

#include <Logger.hpp>
#include <Variables.hpp>
#include <GameStateController.hpp>


GameStateSandbox::GameStateSandbox( GameStateController* const stateController )
  : GameState(stateController)
  , mState(StateLocal::Idle)
  , mPressedKeys()
{}

bool
GameStateSandbox::update( const uint32_t ticks,
                          const TimeUtils::Duration elapsed )
{
  switch ( mState )
  {
    case StateLocal::Idle:
      return true;

    default:
      return true;
  }

  return true;
}

void
GameStateSandbox::keyEvent( const olc::Event event )
{
  const olc::Event::KeyEvent key = event.key;

  switch (event.type)
  {
    case olc::Event::EventType::KeyPressed:
      mPressedKeys.emplace( key.code );
      return;

    case olc::Event::EventType::KeyReleased:
      mPressedKeys.emplace( key.code );
      return;

    default:
      return;
  }
}

void
GameStateSandbox::render()
{

}
