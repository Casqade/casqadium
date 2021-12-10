#include <GameStateSandbox.hpp>

#include <glm/gtx/transform.hpp>

#include <TimeUtils/Duration.hpp>

#include <Widgets.hpp>
#include <Logger.hpp>
#include <Variables.hpp>
#include <GameStateController.hpp>


const float screenWidth = 800.0f;
const float screenHeight = 600.0f;


Camera3D cam( glm::perspective( glm::radians(45.0f), screenWidth / screenHeight, 0.01f, 1000.0f ),
              { 0.0f, 0.0f, screenWidth, screenHeight } );

Poly3D poly( {{{0.0f, 0.0f, 0.0f},
               {0.0f, 1.0f, 0.0f},
               {1.0f, 1.0f, 0.0f},
               {1.0f, 0.0f, 0.0f}}},

               {0.0f, 0.0f, -5.0f} );

GameStateSandbox::GameStateSandbox( GameStateController* const stateController )
  : GameState(stateController)
  , mState(StateLocal::Idle)
  , mPressedKeys()
{}

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
      mPressedKeys.emplace( key.code );
      return;

    default:
      return;
  }
}

void
GameStateSandbox::mouseMoveEvent( const olc::Event::MouseMoveEvent event )
{
  cam.rotatePitch( glm::radians((float) event.dy) );
  cam.rotateYaw( glm::radians((float) event.dx) );
}

void
GameStateSandbox::render()
{
  std::multimap <float, Drawable3D*, std::greater <float>> depthBuffer;
  poly.appendCulled( depthBuffer, cam );

  mPGE->DrawStringDecal({}, std::to_string(cam.orientation().x) + std::to_string(cam.orientation().y) + std::to_string(cam.orientation().z) );

  std::cout << depthBuffer.size() << "\n";
  for ( auto drawable : depthBuffer )
    drawable.second->draw();
}
