#pragma once

#include <GameState.hpp>

#include <cqde/fwd.hpp>

#include <memory>


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
  void setState( const cqde::types::ConfigManager& );

  void clearState();

  void handleEvent( const olc::Event );

  bool update( const uint32_t ticks,
               const TimeUtils::Duration& interval );

  void render( const uint32_t frames,
               const TimeUtils::Duration& interval );
};
