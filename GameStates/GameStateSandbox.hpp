#ifndef GAME_STATE_SANDBOX_HPP
#define GAME_STATE_SANDBOX_HPP

#include <GameState.hpp>
#include <Widgets.hpp>
#include <Graphics3D/Camera.hpp>

#include <set>


class GameStateSandbox
  : public GameState
{
  enum class StateLocal
  {
    Idle,

  } mState;

  Graphics3D::Camera mCamera;
  Poly3D mPolyX;
  Poly3D mPolyY;
  Poly3D mPolyZ;

  std::unique_ptr <olc::Renderable> mPolyXtext;
  std::unique_ptr <olc::Renderable> mPolyYtext;
  std::unique_ptr <olc::Renderable> mPolyZtext;

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
