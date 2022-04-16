#ifndef GAME_STATE_ECS_SANDBOX_HPP
#define GAME_STATE_ECS_SANDBOX_HPP

#include <GameState.hpp>

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

  void handleMouseInput( const int32_t  inputId,
                         const float    inputAmount );

  void handleAxisInput( const float amount,
                        const float direction,
                        const std::string& inputDestinationId );

public:
  GameStateEcsSandbox( GameStateController* const );

  bool update( const uint32_t ticks,
               const TimeUtils::Duration ) override;
  void render() override;
};

#endif
