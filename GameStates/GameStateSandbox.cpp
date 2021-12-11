#include <GameStateSandbox.hpp>

#include <glm/gtx/transform.hpp>

#include <TimeUtils/Duration.hpp>

#include <Widgets.hpp>
#include <Logger.hpp>
#include <Variables.hpp>
#include <GameStateController.hpp>


GameStateSandbox::GameStateSandbox( GameStateController* const stateController )
  : GameState(stateController)
  , mState(StateLocal::Idle)
  , mCamera(glm::perspective( glm::radians(45.0f),
            (float) mPGE->GetWindowSize().x / mPGE->GetWindowSize().y,
            0.01f, 1000.0f ),
            { 0.0f, 0.0f, mPGE->GetWindowSize().x, mPGE->GetWindowSize().y })
//  , mPolyX({{ {0.0f, 0.0f, 0.0f},
//              {0.0f, 1.0f, 0.0f},
//              {1.0f, 1.0f, 0.0f},
//              {1.0f, 0.0f, 0.0f}}})
  , mPolyX({{ {-0.5f, -0.5f, 0.0f},
              {-0.5f, 0.5f, 0.0f},
              {0.5f, 0.5f, 0.0f},
              {0.5f, -0.5f, 0.0f}}})
  , mPolyY(mPolyX)
  , mPolyZ(mPolyX)
  , mPressedKeys()
{
  mPolyX.setColor( olc::RED );
  mPolyY.setColor( olc::GREEN );
  mPolyZ.setColor( olc::BLUE );

  mPolyX.setOrigin({ 5.0f, 0.0f, 0.0f });
  mPolyY.setOrigin({ 0.0f, 5.0f, 0.0f });
  mPolyZ.setOrigin({ 0.0f, 0.0f, 5.0f });
}

bool
GameStateSandbox::update( const uint32_t ticks,
                          const TimeUtils::Duration elapsed )
{
  switch ( mState )
  {
    case StateLocal::Idle:
      return true;

    default:
      return true;
  }

  return true;
}

void
GameStateSandbox::keyEvent( const olc::Event event )
{
  const olc::Event::KeyEvent key = event.key;

  switch (event.type)
  {
    case olc::Event::EventType::KeyPressed:
      mPressedKeys.emplace( key.code );
      return;

    case olc::Event::EventType::KeyReleased:
      mPressedKeys.erase( key.code );
      return;

    default:
      return;
  }
}

void
GameStateSandbox::mouseMoveEvent( const olc::Event::MouseMoveEvent event )
{
  if ( mPressedKeys.size() == 0 )
    return;

  if ( *mPressedKeys.begin() == olc::Key::X )
    mCamera.rotate({ glm::radians((float) event.dy), 0.0f, 0.0f });

  if ( *mPressedKeys.begin() == olc::Key::Y )
    mCamera.rotate({ 0.0f, glm::radians((float) event.dx), 0.0f });

  if ( *mPressedKeys.begin() == olc::Key::Z )
    mCamera.rotate({ 0.0f, 0.0f, glm::radians((float) event.dx) });

  if ( mPressedKeys.count( olc::Key::I ) > 0 )
    mPolyX.rotate( glm::angleAxis( glm::radians((float) event.dx), glm::vec3{0.0f, 1.0f, 0.0f} ) );

  if ( mPressedKeys.count( olc::Key::K ) > 0 )
    mPolyX.rotate( glm::angleAxis( glm::radians((float) event.dx), glm::vec3{0.0f, 0.0f, 1.0f} ) );

  if ( mPressedKeys.count( olc::Key::L ) > 0 )
    mPolyX.rotate( glm::angleAxis( glm::radians((float) event.dy), glm::vec3{1.0f, 0.0f, 0.0f} ) );

}

void
GameStateSandbox::render()
{
  std::multimap <float, Drawable3D*, std::greater <float>> depthBuffer;
  mPolyX.appendCulled( depthBuffer, mCamera );
  mPolyY.appendCulled( depthBuffer, mCamera );
  mPolyZ.appendCulled( depthBuffer, mCamera );

  olc::vf2d textPos = {};

  mPGE->DrawStringDecal({}, "Camera angle:" );
  mPGE->DrawStringDecal(textPos += {0.0f, 10.0f}, std::to_string(mCamera.orientation().x) );
  mPGE->DrawStringDecal(textPos += {0.0f, 10.0f}, std::to_string(mCamera.orientation().y) );
  mPGE->DrawStringDecal(textPos += {0.0f, 10.0f}, std::to_string(mCamera.orientation().z) );

  textPos += {0.0f, 10.0f};
  mPGE->DrawStringDecal(textPos += {0.0f, 10.0f}, "Camera front:" );
  mPGE->DrawStringDecal(textPos += {0.0f, 10.0f}, std::to_string(mCamera.front().x) );
  mPGE->DrawStringDecal(textPos += {0.0f, 10.0f}, std::to_string(mCamera.front().y) );
  mPGE->DrawStringDecal(textPos += {0.0f, 10.0f}, std::to_string(mCamera.front().z) );

  textPos += {0.0f, 10.0f};
  mPGE->DrawStringDecal(textPos += {0.0f, 10.0f}, "Camera right:" );
  mPGE->DrawStringDecal(textPos += {0.0f, 10.0f}, std::to_string(mCamera.right().x) );
  mPGE->DrawStringDecal(textPos += {0.0f, 10.0f}, std::to_string(mCamera.right().y) );
  mPGE->DrawStringDecal(textPos += {0.0f, 10.0f}, std::to_string(mCamera.right().z) );

  textPos += {0.0f, 10.0f};
  mPGE->DrawStringDecal(textPos += {0.0f, 10.0f}, "Camera up:" );
  mPGE->DrawStringDecal(textPos += {0.0f, 10.0f}, std::to_string(mCamera.up().x) );
  mPGE->DrawStringDecal(textPos += {0.0f, 10.0f}, std::to_string(mCamera.up().y) );
  mPGE->DrawStringDecal(textPos += {0.0f, 10.0f}, std::to_string(mCamera.up().z) );

//  std::cout << depthBuffer.size() << "\n";

  for ( auto drawable : depthBuffer )
    drawable.second->draw();
}
