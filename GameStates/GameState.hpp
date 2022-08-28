#pragma once

#include <olcPGE/olcEvent.hpp>


namespace TimeUtils
{
class Duration;
}

class GameStateController;


class GameState
{
protected:
  GameStateController* const mGameStateController;
  olc::PixelGameEngine* const mPGE;

  virtual void keyEvent( const olc::Event );
  virtual void mouseMoveEvent( const olc::Event::MouseMoveEvent );
  virtual void mouseButtonEvent( const olc::Event );
  virtual void mouseWheelEvent( const olc::Event::MouseWheelScrollEvent );
  virtual void windowResizeEvent( const olc::Event::WindowResizeEvent );

public:
  GameState( GameStateController* const );
  virtual ~GameState();

  virtual bool update( const uint32_t ticks,
                       const TimeUtils::Duration& interval );

  virtual void render( const uint32_t frames,
                       const TimeUtils::Duration& interval );

  virtual void handleEvent( const olc::Event );
};
