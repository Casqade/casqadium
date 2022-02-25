#ifndef GAME_STATE_SANDBOX_HPP
#define GAME_STATE_SANDBOX_HPP

#include <GameState.hpp>
#include <Widgets.hpp>
#include <Graphics3D/Camera.hpp>
#include <Graphics3D/Quad.hpp>

#include <set>


class GameStateSandbox
  : public GameState
{
  enum class StateLocal
  {
    Idle,

  } mState;

  Graphics3D::CameraFPS mCamera;

  std::multimap <float, Graphics3D::SceneNode*, std::greater <float>> mDepthBuffer;

  Graphics3D::SceneNode mSceneRoot;

  std::shared_ptr <Graphics3D::Quad> mPolyX;
  std::shared_ptr <Graphics3D::Quad> mPolyY;
  std::shared_ptr <Graphics3D::Quad> mPolyZ;

  std::shared_ptr <Graphics3D::Quad> mAxisX0;
  std::shared_ptr <Graphics3D::Quad> mAxisX1;
  std::shared_ptr <Graphics3D::Quad> mAxisY0;
  std::shared_ptr <Graphics3D::Quad> mAxisY1;
  std::shared_ptr <Graphics3D::Quad> mAxisZ0;
  std::shared_ptr <Graphics3D::Quad> mAxisZ1;

  std::set <Graphics3D::Quad*> mSelectedPolys;

  std::unique_ptr <olc::Renderable> mPolyXtext;
  std::unique_ptr <olc::Renderable> mPolyYtext;
  std::unique_ptr <olc::Renderable> mPolyZtext;

  std::set <olc::Key> mPressedKeys;

  void keyEvent( const olc::Event ) override;
  void mouseMoveEvent( const olc::Event::MouseMoveEvent ) override;
  void mouseButtonEvent( const olc::Event ) override;

public:
  GameStateSandbox( GameStateController* const );

  bool update( const uint32_t ticks,
               const TimeUtils::Duration ) override;
  void render() override;
};


#endif
