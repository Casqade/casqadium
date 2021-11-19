#ifndef GAME_STATE_HPP
#define GAME_STATE_HPP


struct SDL_Renderer;
struct SDL_Event;
struct SDL_KeyboardEvent;
struct SDL_MouseMotionEvent;
struct SDL_MouseButtonEvent;

namespace TimeUtils
{
class Duration;
}

class GameState
{
  enum class StateLocal
  {
    Null,

  } mState;

public:
  ~GameState();
  virtual void enter( );
  virtual void render( SDL_Renderer );
  virtual void update( const TimeUtils::Duration );
  virtual void handleEvent( const SDL_Event );

protected:
  virtual void keyEvent( const SDL_KeyboardEvent );
  virtual void mouseMoveEvent( const SDL_MouseMotionEvent );
  virtual void mouseButtonEvent( const SDL_MouseButtonEvent );
};


#endif
