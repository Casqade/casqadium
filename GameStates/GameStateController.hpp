#ifndef GAME_STATE_CONTROLLER_HPP
#define GAME_STATE_CONTROLLER_HPP

#include <memory>


class GameState;
class GameStateForeword;

class GameStateController
{
  static std::unique_ptr <GameState> mCurrentState;

public:
  template <class NewState>
  static void setState();

  static void handleEvent( const sf::Event );

  static void update( const sf::Time );
  static void render( sf::RenderWindow& );
};


#endif
