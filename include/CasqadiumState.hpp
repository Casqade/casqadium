#pragma once

#include <olcPGE/olcEvent.hpp>


namespace TimeUtils
{
class Duration;
}


class CasqadiumState
{
protected:
  olc::PixelGameEngine* const mPGE;

  virtual void keyEvent( const olc::Event );
  virtual void mouseMoveEvent( const olc::Event::MouseMoveEvent );
  virtual void mouseButtonEvent( const olc::Event );
  virtual void mouseWheelEvent( const olc::Event::MouseWheelScrollEvent );
  virtual void windowResizeEvent( const olc::Event::WindowResizeEvent );

public:
  CasqadiumState();
  virtual ~CasqadiumState();

  virtual bool update( const uint32_t ticks,
                       const TimeUtils::Duration& interval );

  virtual void render( const uint32_t frames,
                       const TimeUtils::Duration& interval );

  virtual void handleEvent( const olc::Event );
};
