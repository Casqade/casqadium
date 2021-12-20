#include <GameStateSandbox.hpp>

#include <glm/gtx/transform.hpp>

#include <TimeUtils/Duration.hpp>
#include <Graphics3D/SceneNode.hpp>

#include <Widgets.hpp>
#include <Logger.hpp>
#include <Variables.hpp>
#include <GameStateController.hpp>


class Node : public Graphics3D::SceneNode
{
  std::string mName;

public:
  Node() = default;
  Node( const std::string& name, const SceneNode* parent = nullptr )
    : Graphics3D::SceneNode(parent)
    , mName(name)
  {}
  Node( Node& ) = default;

  void setName( const std::string& name )
  {
    mName = name;
  }

  std::shared_ptr <SceneNode> clone() override
  {
    return std::make_shared <Node> (*this);
  }
};

GameStateSandbox::GameStateSandbox( GameStateController* const stateController )
  : GameState(stateController)
  , mState(StateLocal::Idle)
  , mCamera()
//  , mPolyX({{ {0.0f, 0.0f, 0.0f},
//              {0.0f, 1.0f, 0.0f},
//              {1.0f, 1.0f, 0.0f},
//              {1.0f, 0.0f, 0.0f}}})
  , mPolyX({{ {-0.5f, 0.5f, 0.0f},
              {-0.5f, -0.5f, 0.0f},
              {0.5f, -0.5f, 0.0f},
              {0.5f, 0.5f, 0.0f}}})
  , mPolyY(mPolyX)
  , mPolyZ(mPolyX)
  , mPolyXtext(std::make_unique <olc::Renderable> ())
  , mPolyYtext(std::make_unique <olc::Renderable> ())
  , mPolyZtext(std::make_unique <olc::Renderable> ())
  , mPressedKeys()
{
  mPolyX.setFrontFace( olc::RED );
  mPolyY.setFrontFace( olc::GREEN );
  mPolyZ.setFrontFace( olc::BLUE );

  mPolyX.setOrigin({ 5.0f, 0.0f, 0.0f });
  mPolyY.setOrigin({ 0.0f, 5.0f, 0.0f });
  mPolyZ.setOrigin({ 0.0f, 0.0f, 5.0f });

  const olc::vi2d textSize = mPGE->GetTextSize( "X" );
  mPolyXtext->Create( textSize.x, textSize.y );
  mPolyYtext->Create( textSize.x, textSize.y );
  mPolyZtext->Create( textSize.x, textSize.y );

  mPGE->SetDrawTarget( mPolyXtext->Sprite() );
  mPGE->Clear( olc::BLANK );
  mPGE->DrawString( 0, 0, "X", olc::RED );

  mPGE->SetDrawTarget( mPolyYtext->Sprite() );
  mPGE->Clear( olc::BLANK );
  mPGE->DrawString( 0, 0, "Y", olc::GREEN );

  mPGE->SetDrawTarget( mPolyZtext->Sprite() );
  mPGE->Clear( olc::BLANK );
  mPGE->DrawString( 0, 0, "Z", olc::BLUE );

  mPGE->SetDrawTarget( nullptr );

  mPolyXtext->Decal()->Update();
  mPolyYtext->Decal()->Update();
  mPolyZtext->Decal()->Update();

  mPolyX.setFrontFace( mPolyXtext->Decal() );
  mPolyY.setFrontFace( mPolyYtext->Decal() );
  mPolyZ.setFrontFace( mPolyZtext->Decal() );

  mPolyX.setScale({ 1.0f, 2.0f, 2.0f });

  mCamera.setViewport({ 0.0f, 0.0f, mPGE->GetWindowSize().x, mPGE->GetWindowSize().y });
}

bool
GameStateSandbox::update( const uint32_t ticks,
                          const TimeUtils::Duration elapsed )
{
  if ( mPressedKeys.count( olc::Key::ESCAPE ) > 0 )
    return false;

  if ( mPGE->GetKey( olc::Key::W ).bHeld )
    mCamera.translate( mCamera.front() );

  if ( mPGE->GetKey( olc::Key::S ).bHeld )
    mCamera.translate( -mCamera.front() );

  if ( mPGE->GetKey( olc::Key::A ).bHeld )
    mCamera.translate( -mCamera.right() );

  if ( mPGE->GetKey( olc::Key::D ).bHeld )
    mCamera.translate( mCamera.right() );

  if ( mPGE->GetKey( olc::Key::SPACE ).bHeld )
    mCamera.translate( mCamera.up() );

  if ( mPGE->GetKey( olc::Key::SHIFT ).bHeld )
    mCamera.translate( -mCamera.up() );

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
    mCamera.rotate( glm::angleAxis( glm::radians((float) -event.dy), glm::vec3{1.0f, 0.0f, 0.0f} ) );

  if ( mPressedKeys.count( olc::Key::Z ) > 0 )
    mCamera.rotate( glm::angleAxis( glm::radians((float) -event.dx), glm::vec3{0.0f, 0.0f, 1.0f} ) );
  else
    //  if ( mPressedKeys.count( olc::Key::Y ) > 0 )
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

  const glm::vec3 camPos = mCamera.origin();
  const glm::vec3 camOrientation = glm::degrees(glm::eulerAngles( mCamera.orientation() ));
  const glm::vec3 camFront = glm::degrees(mCamera.front());
  const glm::vec3 camRight = glm::degrees(mCamera.right());
  const glm::vec3 camUp = glm::degrees(mCamera.up());

  olc::vf2d textPos = {};

  mPGE->DrawStringDecal( {}, "Camera pos:" );
  mPGE->DrawStringDecal( textPos += {0.0f, 10.0f}, std::to_string(camPos.x) );
  mPGE->DrawStringDecal( textPos += {0.0f, 10.0f}, std::to_string(camPos.y) );
  mPGE->DrawStringDecal( textPos += {0.0f, 10.0f}, std::to_string(camPos.z) );

  textPos += {0.0f, 10.0f};
  mPGE->DrawStringDecal( textPos += {0.0f, 10.0f}, "Camera angle:" );
  mPGE->DrawStringDecal( textPos += {0.0f, 10.0f}, std::to_string(camOrientation.x) );
  mPGE->DrawStringDecal( textPos += {0.0f, 10.0f}, std::to_string(camOrientation.y) );
  mPGE->DrawStringDecal( textPos += {0.0f, 10.0f}, std::to_string(camOrientation.z) );

  textPos += {0.0f, 10.0f};
  mPGE->DrawStringDecal( textPos += {0.0f, 10.0f}, "Camera front:" );
  mPGE->DrawStringDecal( textPos += {0.0f, 10.0f}, std::to_string(camFront.x) );
  mPGE->DrawStringDecal( textPos += {0.0f, 10.0f}, std::to_string(camFront.y) );
  mPGE->DrawStringDecal( textPos += {0.0f, 10.0f}, std::to_string(camFront.z) );

  textPos += {0.0f, 10.0f};
  mPGE->DrawStringDecal( textPos += {0.0f, 10.0f}, "Camera right:" );
  mPGE->DrawStringDecal( textPos += {0.0f, 10.0f}, std::to_string(camRight.x) );
  mPGE->DrawStringDecal( textPos += {0.0f, 10.0f}, std::to_string(camRight.y) );
  mPGE->DrawStringDecal( textPos += {0.0f, 10.0f}, std::to_string(camRight.z) );

  textPos += {0.0f, 10.0f};
  mPGE->DrawStringDecal( textPos += {0.0f, 10.0f}, "Camera up:" );
  mPGE->DrawStringDecal( textPos += {0.0f, 10.0f}, std::to_string(camUp.x) );
  mPGE->DrawStringDecal( textPos += {0.0f, 10.0f}, std::to_string(camUp.y) );
  mPGE->DrawStringDecal( textPos += {0.0f, 10.0f}, std::to_string(camUp.z) );

  textPos += {0.0f, 10.0f};
  mPGE->DrawStringDecal( textPos += {0.0f, 10.0f}, "Depth buffer size: " + std::to_string(depthBuffer.size()) );

  for ( auto drawable : depthBuffer )
    drawable.second->draw();
}
