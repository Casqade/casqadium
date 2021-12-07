#ifndef GAME_STATE_CONTROLLER_HPP
#define GAME_STATE_CONTROLLER_HPP

#include <memory>
#include <GameState.hpp>


namespace TimeUtils
{
class Duration;
}

namespace olc
{
class PixelGameEngine;
class Event;
}

class GameStateController
{
  std::unique_ptr <GameState> mCurrentState;

public:
  GameStateController() = default;

  template <class NewState>
  void setState();

  void handleEvent( const olc::Event );

  bool update( const uint32_t ticks,
               const TimeUtils::Duration );
  void render();
};


#endif
