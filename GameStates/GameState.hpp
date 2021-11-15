#ifndef GAME_STATE_HPP
#define GAME_STATE_HPP

#include <SFML/Window/Event.hpp>


namespace sf
{
class RenderWindow;
class Time;
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
  virtual void render( sf::RenderWindow& );
  virtual void update( const sf::Time );
  virtual void handleEvent( const sf::Event );

protected:
  virtual void keyEvent( const sf::Event );
  virtual void mouseMoveEvent( const sf::Event::MouseMoveEvent );
  virtual void mouseButtonEvent( const sf::Event::MouseButtonEvent );
};


#endif
