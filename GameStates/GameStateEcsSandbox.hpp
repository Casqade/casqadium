#pragma once

#include <GameState.hpp>

#include <cqde/alias.hpp>

#include <entt/entt.hpp>


class GameStateEcsSandbox
  : public GameState
{
  enum class StateLocal
  {
    Idle,

  } mState;

  entt::registry mRegistry;

  bool mRunning;

  void keyEvent( const olc::Event ) override;
  void mouseMoveEvent( const olc::Event::MouseMoveEvent ) override;
  void mouseButtonEvent( const olc::Event ) override;

  void handleAxisInput( const cqde::InputHwId inputSrc,
                        const float amount,
                        const float direction );

public:
  GameStateEcsSandbox( GameStateController* const );

  bool update( const uint32_t ticks,
               const TimeUtils::Duration ) override;
  void render() override;
};
