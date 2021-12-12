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
  if ( mPGE->GetKey( olc::Key::W ).bHeld )
    mCamera.move( mCamera.front() );

  if ( mPGE->GetKey( olc::Key::S ).bHeld )
    mCamera.move( -mCamera.front() );

  if ( mPGE->GetKey( olc::Key::A ).bHeld )
    mCamera.move( -mCamera.right() );

  if ( mPGE->GetKey( olc::Key::D ).bHeld )
    mCamera.move( mCamera.right() );

  if ( mPGE->GetKey( olc::Key::SPACE ).bHeld )
    mCamera.move( mCamera.up() );

  if ( mPGE->GetKey( olc::Key::SHIFT ).bHeld )
    mCamera.move( -mCamera.up() );

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

  if ( mPressedKeys.size() == 0 )
    return;
}

void
GameStateSandbox::mouseMoveEvent( const olc::Event::MouseMoveEvent event )
{
  if ( mPressedKeys.count( olc::Key::CTRL ) == 0 )
  {
//  if ( mPressedKeys.count( olc::Key::X ) > 0 )
//    mCamera.rotate( glm::vec4(mCamera.right().x, mCamera.right().y, mCamera.right().z, glm::radians((float) event.dy)) );
    mCamera.rotate( glm::angleAxis( glm::radians((float) -event.dy), glm::vec3{1.0f, 0.0f, 0.0f} ) );

  if ( mPressedKeys.count( olc::Key::Z ) > 0 )
  //    mCamera.rotate( glm::vec4(mCamera.front().x, mCamera.front().y, mCamera.front().z, glm::radians((float) event.dx)) );
    mCamera.rotate( glm::angleAxis( glm::radians((float) -event.dx), glm::vec3{0.0f, 0.0f, 1.0f} ) );
  else
    //  if ( mPressedKeys.count( olc::Key::Y ) > 0 )
//    mCamera.rotate( glm::vec4(mCamera.up().x, mCamera.up().y, mCamera.up().z, glm::radians((float) -event.dx)) );
    mCamera.rotate( glm::angleAxis( glm::radians((float) -event.dx), glm::vec3{0.0f, 1.0f, 0.0f} ) );

  }

  if ( mPressedKeys.size() == 0 )
    return;

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

  mPGE->DrawStringDecal( {}, "Camera angle:" );
  mPGE->DrawStringDecal( textPos += {0.0f, 10.0f}, std::to_string(mCamera.orientation().x) );
  mPGE->DrawStringDecal( textPos += {0.0f, 10.0f}, std::to_string(mCamera.orientation().y) );
  mPGE->DrawStringDecal( textPos += {0.0f, 10.0f}, std::to_string(mCamera.orientation().z) );

  textPos += {0.0f, 10.0f};
  mPGE->DrawStringDecal( textPos += {0.0f, 10.0f}, "Camera front:" );
  mPGE->DrawStringDecal( textPos += {0.0f, 10.0f}, std::to_string(mCamera.front().x) );
  mPGE->DrawStringDecal( textPos += {0.0f, 10.0f}, std::to_string(mCamera.front().y) );
  mPGE->DrawStringDecal( textPos += {0.0f, 10.0f}, std::to_string(mCamera.front().z) );

  textPos += {0.0f, 10.0f};
  mPGE->DrawStringDecal( textPos += {0.0f, 10.0f}, "Camera right:" );
  mPGE->DrawStringDecal( textPos += {0.0f, 10.0f}, std::to_string(mCamera.right().x) );
  mPGE->DrawStringDecal( textPos += {0.0f, 10.0f}, std::to_string(mCamera.right().y) );
  mPGE->DrawStringDecal( textPos += {0.0f, 10.0f}, std::to_string(mCamera.right().z) );

  textPos += {0.0f, 10.0f};
  mPGE->DrawStringDecal( textPos += {0.0f, 10.0f}, "Camera up:" );
  mPGE->DrawStringDecal( textPos += {0.0f, 10.0f}, std::to_string(mCamera.up().x) );
  mPGE->DrawStringDecal( textPos += {0.0f, 10.0f}, std::to_string(mCamera.up().y) );
  mPGE->DrawStringDecal( textPos += {0.0f, 10.0f}, std::to_string(mCamera.up().z) );

//  std::cout << depthBuffer.size() << "\n";

  for ( auto drawable : depthBuffer )
    drawable.second->draw();
}
