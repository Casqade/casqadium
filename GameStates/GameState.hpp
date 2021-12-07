#ifndef GAME_STATE_HPP
#define GAME_STATE_HPP

#include <olcPGE/olcEvent.hpp>


namespace TimeUtils
{
class Duration;
}

class GameStateController;


class GameState
{
  enum class StateLocal
  {
    Null,

  } mState;

protected:
  GameStateController* const mGameStateController;

  virtual void keyEvent( const olc::Event );
  virtual void mouseMoveEvent( const olc::Event::MouseMoveEvent );
  virtual void mouseButtonEvent( const olc::Event::MouseButtonEvent );

public:
  GameState( GameStateController* const );
  virtual ~GameState();

  virtual void enter();

  virtual bool update( const uint32_t ticks,
                       const TimeUtils::Duration );

  virtual void render( olc::PixelGameEngine* const );

  virtual void handleEvent( const olc::Event );
};


#endif
