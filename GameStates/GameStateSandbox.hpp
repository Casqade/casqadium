#ifndef GAME_STATE_SANDBOX_HPP
#define GAME_STATE_SANDBOX_HPP

#include <GameState.hpp>

#include <set>


class GameStateSandbox
  : public GameState
{
  enum class StateLocal
  {
    Idle,

  } mState;

  std::set <olc::Key> mPressedKeys;

  void keyEvent( const olc::Event ) override;
  void mouseMoveEvent( const olc::Event::MouseMoveEvent ) override;

public:
  GameStateSandbox( GameStateController* const );

  bool update( const uint32_t ticks,
               const TimeUtils::Duration ) override;
  void render() override;
};


#endif
