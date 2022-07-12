#pragma once

#include <GameState.hpp>

#include <cqde/alias.hpp>

#include <entt/entt.hpp>


class GameStateEcsSandbox
  : public GameState
{
  entt::registry mRegistry {};

  bool mRunning {true};

  void keyEvent( const olc::Event ) override;
  void mouseMoveEvent( const olc::Event::MouseMoveEvent ) override;
  void mouseButtonEvent( const olc::Event ) override;

public:
  GameStateEcsSandbox( GameStateController* const );

  bool update( const uint32_t ticks,
               const TimeUtils::Duration& interval ) override;
  void render( const uint32_t frames,
               const TimeUtils::Duration& interval ) override;
};
