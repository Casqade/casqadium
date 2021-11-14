#ifndef GAME_STATE_TITLE_HPP
#define GAME_STATE_TITLE_HPP

#include "GameState.hpp"


namespace sf
{
class Time;
class RenderWindow;
}

class GameStateTitle
  : public GameState
{
  enum class StateLocal
  {
      Foreword
    , ForewordWaitAccept
    , Title
    ,

  };

public:
//  void enter() override;
  void update( const sf::Time ) override;
  void render( sf::RenderWindow& ) override;
};


#endif
