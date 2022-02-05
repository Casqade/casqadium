#include <GameStateSandbox.hpp>

#include <glm/gtx/transform.hpp>

#include <TimeUtils/Duration.hpp>
#include <Graphics3D/SceneNode.hpp>

#include <Widgets.hpp>
#include <Logger.hpp>
#include <Variables.hpp>
#include <GameStateController.hpp>

#include <imgui/imgui.h>


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
  , mPolyX({{ {-0.5f, 0.5f, 0.0f},
              {-0.5f, -0.5f, 0.0f},
              {0.5f, -0.5f, 0.0f},
              {0.5f, 0.5f, 0.0f}}})
  , mPolyY(mPolyX)
  , mPolyZ(mPolyX)
  , mAxisX0(mPolyX)
  , mAxisX1(mPolyX)
  , mAxisY0(mPolyX)
  , mAxisY1(mPolyX)
  , mAxisZ0(mPolyX)
  , mAxisZ1(mPolyX)
  , mSelectedPolys()
  , mPolyXtext(std::make_unique <olc::Renderable> ())
  , mPolyYtext(std::make_unique <olc::Renderable> ())
  , mPolyZtext(std::make_unique <olc::Renderable> ())
  , mPressedKeys()
{
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

  mPolyX.setFrontFace( olc::RED );
  mPolyY.setFrontFace( olc::GREEN );
  mPolyZ.setFrontFace( olc::BLUE );

  mPolyX.setOrigin({ 5.0f, 0.0f, 0.0f });
  mPolyY.setOrigin({ 0.0f, 5.0f, 0.0f });
  mPolyZ.setOrigin({ 0.0f, 0.0f, 5.0f });

  mAxisX0.setOrigin({ -100.0f, 0.0f, 0.0f });
  mAxisX1.setOrigin({ 100.0f, 0.0f, 0.0f });
  mAxisY0.setOrigin({ 0.0f, -100.0f, 0.0f });
  mAxisY1.setOrigin({ 0.0f, 100.0f, 0.0f });
  mAxisZ0.setOrigin({ 0.0f, 0.0f, -100.0f });
  mAxisZ1.setOrigin({ 0.0f, 0.0f, 100.0f });
  auto gizmo = std::make_shared <Graphics3D::OrientationGizmo> (&mCamera);
  gizmo->setScale({20.0f, 20.0f, 20.0f});
//  gizmo->setOrigin({0.0f, 0.0f, -2.0f});

  mPolyX.setFrontFace( mPolyXtext->Decal() );
  mPolyY.setFrontFace( mPolyYtext->Decal() );
  mPolyZ.setFrontFace( mPolyZtext->Decal() );

  mAxisX0.setFrontFace( mPolyXtext->Decal() );
  mAxisX1.setFrontFace( mPolyXtext->Decal() );
  mAxisY0.setFrontFace( mPolyYtext->Decal() );
  mAxisY1.setFrontFace( mPolyYtext->Decal() );
  mAxisZ0.setFrontFace( mPolyZtext->Decal() );
  mAxisZ1.setFrontFace( mPolyZtext->Decal() );

  mAxisX0.setScale({ 50.0f, 50.0f, 0.0f });
  mAxisX1.setScale({ 50.0f, 50.0f, 0.0f });
  mAxisY0.setScale({ 50.0f, 50.0f, 0.0f });
  mAxisY1.setScale({ 50.0f, 50.0f, 0.0f });
  mAxisZ0.setScale({ 50.0f, 50.0f, 0.0f });
  mAxisZ1.setScale({ 50.0f, 50.0f, 0.0f });

  mAxisX0.rotateGlobal(glm::angleAxis(glm::radians(90.0f), glm::vec3{0.0f, 1.0f, 0.0f}));
  mAxisX1.rotateGlobal(glm::angleAxis(glm::radians(-90.0f), glm::vec3{0.0f, 1.0f, 0.0f}));
  mAxisY0.rotateGlobal(glm::angleAxis(glm::radians(-90.0f), glm::vec3{1.0f, 0.0f, 0.0f}));
  mAxisY1.rotateGlobal(glm::angleAxis(glm::radians(90.0f), glm::vec3{1.0f, 0.0f, 0.0f}));
  mAxisZ0.rotateGlobal(glm::angleAxis(glm::radians(0.0f), glm::vec3{0.0f, 1.0f, 0.0f}));
  mAxisZ1.rotateGlobal(glm::angleAxis(glm::radians(180.0f), glm::vec3{0.0f, 1.0f, 0.0f}));

  mCamera.setViewport({ 0.0f, 0.0f, mPGE->GetWindowSize().x, mPGE->GetWindowSize().y });
//  mCamera.setProjection(Graphics3D::Camera::Projection::Orthogonal);

  mCameraRoot.addChild(gizmo);
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

  if ( mPGE->GetKey( olc::Key::C ).bPressed )
  {
    static olc::vi2d cursor = {};
    mPolyZtext->Sprite()->SetPixel(cursor.x, cursor.y, olc::DARK_BLUE);
    mPolyZtext->Decal()->Update();
    ++cursor.x %= mPolyZtext->Sprite()->width;
    cursor.y = cursor.x == 0 ? ++cursor.y %= mPolyZtext->Sprite()->height : cursor.y;
  }

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
  if ( mPressedKeys.count( olc::Key::CTRL ) > 0 )
  {
//  if ( mPressedKeys.count( olc::Key::X ) > 0 )
    mCamera.rotate( glm::angleAxis( glm::radians((float) -event.dy), glm::vec3{1.0f, 0.0f, 0.0f} ) );

  if ( mPressedKeys.count( olc::Key::Z ) > 0 )
    mCamera.rotate( glm::angleAxis( glm::radians((float) -event.dx), glm::vec3{0.0f, 0.0f, 1.0f} ) );
  else
    //  if ( mPressedKeys.count( olc::Key::Y ) > 0 )
    mCamera.rotateGlobal( glm::angleAxis( glm::radians((float) -event.dx), glm::vec3{0.0f, 1.0f, 0.0f} ) );
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
GameStateSandbox::mouseButtonEvent( const olc::Event event )
{
  if ( event.type != olc::Event::MouseButtonPressed )
    return;

  if ( event.mouseButton.button == olc::Event::MouseButton::Left )
  {
    std::for_each( mDepthBuffer.rbegin(), mDepthBuffer.rend(),
    [&] ( const auto& node )
    {
      auto* poly = dynamic_cast <Graphics3D::Quad*> ( node.second );
      if ( poly && poly->isUnderCursor({ event.mouseButton.x, event.mouseButton.y }) )
      {
        if ( mSelectedPolys.count( poly ) > 0 )
        {
          mSelectedPolys.erase( poly );
          return poly->setSelected( false );
        }

        poly->setSelected( true );
        mSelectedPolys.insert( poly );

        return;
      }
    });
  }

  if ( event.mouseButton.button == olc::Event::MouseButton::Right )
  {
    for ( auto poly : mSelectedPolys )
      poly->setSelected( false );

    mSelectedPolys.clear();
  }
}

void
GameStateSandbox::render()
{
  mDepthBuffer.clear();

  mPolyX.appendCulled( mDepthBuffer, &mCamera );
  mPolyY.appendCulled( mDepthBuffer, &mCamera );
  mPolyZ.appendCulled( mDepthBuffer, &mCamera );

//  mAxisX0.appendCulled( mDepthBuffer, &mCamera );
//  mAxisX1.appendCulled( mDepthBuffer, &mCamera );
//  mAxisY0.appendCulled( mDepthBuffer, &mCamera );
//  mAxisY1.appendCulled( mDepthBuffer, &mCamera );
//  mAxisZ0.appendCulled( mDepthBuffer, &mCamera );
//  mAxisZ1.appendCulled( mDepthBuffer, &mCamera );
  mCameraRoot.appendCulled( mDepthBuffer, &mCamera );

  const glm::vec3 camPos = mCamera.origin();
  const glm::vec3 camOrientation = glm::degrees(glm::eulerAngles( mCamera.orientation() ));
  const glm::vec3 camFront = glm::degrees(mCamera.front());
  const glm::vec3 camRight = glm::degrees(mCamera.right());
  const glm::vec3 camUp = glm::degrees(mCamera.up());

  ImGui::SetNextWindowPos({mPGE->GetWindowSize().x * 0.9f, 0.0f});

  ImGui::Text("Camera pos:");
  ImGui::TextColored({1.0f, 0.0f, 0.0f, 1.0f}, std::to_string(camPos.x).c_str());
  ImGui::TextColored({0.0f, 1.0f, 0.0f, 1.0f}, std::to_string(camPos.y).c_str());
  ImGui::TextColored({0.0f, 0.0f, 1.0f, 1.0f}, std::to_string(camPos.z).c_str());

  ImGui::Text("Camera angle:");
  ImGui::TextColored({1.0f, 0.0f, 0.0f, 1.0f}, std::to_string(camOrientation.x).c_str());
  ImGui::TextColored({0.0f, 1.0f, 0.0f, 1.0f}, std::to_string(camOrientation.y).c_str());
  ImGui::TextColored({0.0f, 0.0f, 1.0f, 1.0f}, std::to_string(camOrientation.z).c_str());

  ImGui::Text("Camera front:");
  ImGui::TextColored({1.0f, 0.0f, 0.0f, 1.0f}, std::to_string(camFront.x).c_str());
  ImGui::TextColored({0.0f, 1.0f, 0.0f, 1.0f}, std::to_string(camFront.y).c_str());
  ImGui::TextColored({0.0f, 0.0f, 1.0f, 1.0f}, std::to_string(camFront.z).c_str());

  ImGui::Text("Camera right:");
  ImGui::TextColored({1.0f, 0.0f, 0.0f, 1.0f}, std::to_string(camRight.x).c_str());
  ImGui::TextColored({0.0f, 1.0f, 0.0f, 1.0f}, std::to_string(camRight.y).c_str());
  ImGui::TextColored({0.0f, 0.0f, 1.0f, 1.0f}, std::to_string(camRight.z).c_str());

  ImGui::Text("Camera up:");
  ImGui::TextColored({1.0f, 0.0f, 0.0f, 1.0f}, std::to_string(camUp.x).c_str());
  ImGui::TextColored({0.0f, 1.0f, 0.0f, 1.0f}, std::to_string(camUp.y).c_str());
  ImGui::TextColored({0.0f, 0.0f, 1.0f, 1.0f}, std::to_string(camUp.z).c_str());

  ImGui::Text(("Depth buffer size: " + std::to_string(mDepthBuffer.size())).c_str());

  for ( auto drawable : mDepthBuffer )
    drawable.second->draw();
}
