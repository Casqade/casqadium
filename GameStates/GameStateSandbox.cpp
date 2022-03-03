#include <GameStateSandbox.hpp>

#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <TimeUtils/Duration.hpp>
#include <Graphics3D/SceneNode.hpp>

#include <Widgets.hpp>
#include <Logger.hpp>
#include <Variables.hpp>
#include <GameStateController.hpp>

#include <imgui/imgui.h>
#include <ImGuizmo/ImGuizmo.h>


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
  , mPolyX(std::make_shared <Graphics3D::Quad> ())
  , mPolyY(std::make_shared <Graphics3D::Quad> (*mPolyX))
  , mPolyZ(std::make_shared <Graphics3D::Quad> (*mPolyX))
  , mAxisX0(std::make_shared <Graphics3D::Quad> (*mPolyX))
  , mAxisX1(std::make_shared <Graphics3D::Quad> (*mPolyX))
  , mAxisY0(std::make_shared <Graphics3D::Quad> (*mPolyX))
  , mAxisY1(std::make_shared <Graphics3D::Quad> (*mPolyX))
  , mAxisZ0(std::make_shared <Graphics3D::Quad> (*mPolyX))
  , mAxisZ1(std::make_shared <Graphics3D::Quad> (*mPolyX))
  , mSelectedPolys()
  , mPolyXtext(std::make_unique <olc::Renderable> ())
  , mPolyYtext(std::make_unique <olc::Renderable> ())
  , mPolyZtext(std::make_unique <olc::Renderable> ())
  , mPressedKeys()
{
//  mCamera.setProjection(Graphics3D::Camera::Projection::Orthographic);
//  mCamera.setViewport({-10.0f, 10.0f, -10.0f, 10.0f });
  mCamera.setViewport({ 0.0f, 0.0f, mPGE->GetWindowSize().x, mPGE->GetWindowSize().y });

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

  mPolyX->setFrontFace( olc::RED );
  mPolyY->setFrontFace( olc::GREEN );
  mPolyZ->setFrontFace( olc::BLUE );

  mPolyX->setOrigin({ 5.0f, 0.0f, 0.0f });
  mPolyY->setOrigin({ 0.0f, 5.0f, 0.0f });
  mPolyZ->setOrigin({ 0.0f, 0.0f, 5.0f });

  mAxisX0->setOrigin({ -100.0f, 0.0f, 0.0f });
  mAxisX1->setOrigin({ 100.0f, 0.0f, 0.0f });
  mAxisY0->setOrigin({ 0.0f, -100.0f, 0.0f });
  mAxisY1->setOrigin({ 0.0f, 100.0f, 0.0f });
  mAxisZ0->setOrigin({ 0.0f, 0.0f, -100.0f });
  mAxisZ1->setOrigin({ 0.0f, 0.0f, 100.0f });

  mPolyX->setFrontFace( mPolyXtext->Decal() );
  mPolyY->setFrontFace( mPolyYtext->Decal() );
  mPolyZ->setFrontFace( mPolyZtext->Decal() );

  mAxisX0->setFrontFace( mPolyXtext->Decal() );
  mAxisX1->setFrontFace( mPolyXtext->Decal() );
  mAxisY0->setFrontFace( mPolyYtext->Decal() );
  mAxisY1->setFrontFace( mPolyYtext->Decal() );
  mAxisZ0->setFrontFace( mPolyZtext->Decal() );
  mAxisZ1->setFrontFace( mPolyZtext->Decal() );

  mAxisX0->setScale({ 50.0f, 50.0f, 50.0f });
  mAxisX1->setScale({ 50.0f, 50.0f, 50.0f });
  mAxisY0->setScale({ 50.0f, 50.0f, 50.0f });
  mAxisY1->setScale({ 50.0f, 50.0f, 50.0f });
  mAxisZ0->setScale({ 50.0f, 50.0f, 50.0f });
  mAxisZ1->setScale({ 50.0f, 50.0f, 50.0f });

  mAxisX0->rotateGlobal(glm::angleAxis(glm::radians(90.0f), glm::vec3{0.0f, 1.0f, 0.0f}));
  mAxisX1->rotateGlobal(glm::angleAxis(glm::radians(-90.0f), glm::vec3{0.0f, 1.0f, 0.0f}));
  mAxisY0->rotateGlobal(glm::angleAxis(glm::radians(-90.0f), glm::vec3{1.0f, 0.0f, 0.0f}));
  mAxisY1->rotateGlobal(glm::angleAxis(glm::radians(90.0f), glm::vec3{1.0f, 0.0f, 0.0f}));
  mAxisZ0->rotateGlobal(glm::angleAxis(glm::radians(0.0f), glm::vec3{0.0f, 1.0f, 0.0f}));
  mAxisZ1->rotateGlobal(glm::angleAxis(glm::radians(180.0f), glm::vec3{0.0f, 1.0f, 0.0f}));

  mSceneRoot.addChild(mPolyX);
  mSceneRoot.addChild(mPolyY);
  mSceneRoot.addChild(mPolyZ);

  mSceneRoot.addChild(mAxisX0);
  mSceneRoot.addChild(mAxisX1);
  mSceneRoot.addChild(mAxisY0);
  mSceneRoot.addChild(mAxisY1);
  mSceneRoot.addChild(mAxisZ0);
  mSceneRoot.addChild(mAxisZ1);
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
    mCamera.rotate( glm::normalize(glm::angleAxis( glm::radians((float) -event.dy), glm::vec3{1.0f, 0.0f, 0.0f} )) );
//    mCamera.rotateGlobal( glm::normalize(glm::angleAxis( glm::radians((float) -event.dy), mCamera.toWorldSpace(mCamera.right()) )) );

  if ( mPressedKeys.count( olc::Key::Z ) > 0 )
    mCamera.rotate( glm::normalize(glm::angleAxis( glm::radians((float) -event.dx), glm::vec3{0.0f, 0.0f, 1.0f} )) );
  else
//      if ( mPressedKeys.count( olc::Key::Y ) > 0 )
    mCamera.rotateGlobal( glm::normalize(glm::angleAxis( glm::radians((float) -event.dx), glm::vec3{0.0f, 1.0f, 0.0f} )) );
  }

  if ( mPressedKeys.size() == 0 )
    return;

  if ( mPressedKeys.count( olc::Key::I ) > 0 )
    mPolyX->rotate( glm::angleAxis( glm::radians((float) event.dx), glm::vec3{0.0f, 1.0f, 0.0f} ) );

  if ( mPressedKeys.count( olc::Key::K ) > 0 )
    mPolyX->rotate( glm::angleAxis( glm::radians((float) event.dx), glm::vec3{0.0f, 0.0f, 1.0f} ) );

  if ( mPressedKeys.count( olc::Key::L ) > 0 )
    mPolyX->rotate( glm::angleAxis( glm::radians((float) event.dy), glm::vec3{1.0f, 0.0f, 0.0f} ) );
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
          return;
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

  mSceneRoot.appendCulled( mDepthBuffer, &mCamera );

  const glm::vec3 camPos = mCamera.origin();
  const glm::vec3 camOrientation = glm::degrees(glm::eulerAngles( mCamera.orientation() ));
  const glm::quat camQuat = mCamera.orientation();
  const glm::vec3 camFront = mCamera.front();
  const glm::vec3 camRight = mCamera.right();
//  const glm::vec3 camUp = glm::row( mCamera.viewMatrix(), Graphics3D::Camera::ViewMatrixVector::Up );
  const glm::vec3 camUp = mCamera.toWorldSpace( mCamera.up() );

  ImGui::SetNextWindowPos({mPGE->GetDrawTargetWidth() * 0.9f, 0.0f});
  ImGui::Begin("Orientation");

  ImGui::Text("Camera pos:");
  ImGui::TextColored({1.0f, 0.0f, 0.0f, 1.0f}, std::to_string(camPos.x).c_str());
  ImGui::TextColored({0.0f, 1.0f, 0.0f, 1.0f}, std::to_string(camPos.y).c_str());
  ImGui::TextColored({0.0f, 0.0f, 1.0f, 1.0f}, std::to_string(camPos.z).c_str());

  ImGui::Text("Camera quat:");
  ImGui::TextColored({1.0f, 0.0f, 0.0f, 1.0f}, std::to_string(camQuat.x).c_str());
  ImGui::TextColored({0.0f, 1.0f, 0.0f, 1.0f}, std::to_string(camQuat.y).c_str());
  ImGui::TextColored({0.0f, 0.0f, 1.0f, 1.0f}, std::to_string(camQuat.z).c_str());
  ImGui::TextColored({1.0f, 1.0f, 1.0f, 1.0f}, std::to_string(camQuat.w).c_str());

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

  ImGui::End();


  static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::TRANSLATE);
  static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::LOCAL);

  auto camView = mCamera.viewMatrix();

  if ( mSelectedPolys.size() > 0 )
  {
    ImGui::SetNextWindowPos({0.0f, 128.0f});
    ImGui::Begin("Gizmo");

    if ( ImGui::RadioButton("Translate", mCurrentGizmoOperation == ImGuizmo::TRANSLATE) )
      mCurrentGizmoOperation = ImGuizmo::TRANSLATE;

    ImGui::SameLine();
    if ( ImGui::RadioButton("Rotate", mCurrentGizmoOperation == ImGuizmo::ROTATE) )
      mCurrentGizmoOperation = ImGuizmo::ROTATE;

    ImGui::SameLine();
    if ( ImGui::RadioButton("Scale", mCurrentGizmoOperation == ImGuizmo::SCALE) )
    {
      mCurrentGizmoMode = ImGuizmo::LOCAL;
      mCurrentGizmoOperation = ImGuizmo::SCALE;
    }

    auto node = *mSelectedPolys.begin();
    glm::mat4 nodeMat;

    if ( mCurrentGizmoMode == ImGuizmo::LOCAL )
      nodeMat = node->modelLocal();
    else if ( mCurrentGizmoMode == ImGuizmo::WORLD )
      nodeMat = node->modelWorld();

    glm::vec3 nodeTrans;
    glm::quat nodeRotation;
    glm::vec3 nodeScale = node->scale();
    glm::vec3 skew;
    glm::vec4 pers;

    glm::decompose(nodeMat,
                   nodeScale,
                   nodeRotation,
                   nodeTrans,
                   skew,
                   pers);

    glm::vec3 nodeOrien = glm::degrees(glm::eulerAngles(nodeRotation));

    if ( ImGui::InputFloat3("Tr", glm::value_ptr(nodeTrans)) )
    {
      if ( mCurrentGizmoMode == ImGuizmo::LOCAL )
        node->setOrigin(nodeTrans);

      else if ( mCurrentGizmoMode == ImGuizmo::WORLD )
        node->setOrigin(node->toLocalSpace(nodeTrans));
    }
    if ( ImGui::InputFloat3("Rt", glm::value_ptr(nodeOrien)) )
    {
      if ( mCurrentGizmoMode == ImGuizmo::LOCAL )
        node->setOrientation(glm::radians(nodeOrien));

      else if ( mCurrentGizmoMode == ImGuizmo::WORLD )
        node->setOrientation(node->toLocalSpace(glm::quat(glm::radians(nodeOrien))));
    }
    if ( ImGui::InputFloat3("Sc", glm::value_ptr(nodeScale)) )
      node->setScale(nodeScale);

    if ( ImGui::RadioButton("Local", mCurrentGizmoMode == ImGuizmo::LOCAL) )
      mCurrentGizmoMode = ImGuizmo::LOCAL;

    if (mCurrentGizmoOperation != ImGuizmo::SCALE)
    {
      ImGui::SameLine();
      if ( ImGui::RadioButton("World", mCurrentGizmoMode == ImGuizmo::WORLD) )
        mCurrentGizmoMode = ImGuizmo::WORLD;
    }

    static bool useSnap = false;
    static float snap[3] = { 1.f, 1.f, 1.f };

    ImGui::Checkbox("##", &useSnap);
    ImGui::SameLine();
    switch (mCurrentGizmoOperation)
    {
      case ImGuizmo::TRANSLATE:
        ImGui::InputFloat3("Snap", &snap[0]);
        break;

      case ImGuizmo::ROTATE:
        ImGui::InputFloat("Angle Snap", &snap[0]);
        break;

      case ImGuizmo::SCALE:
        ImGui::InputFloat("Scale Snap", &snap[0]);
        break;

      default:
        break;
    }

    nodeMat = node->modelWorld();
    auto camProj = mCamera.projMatrix();
    auto identity = glm::mat4(1.0f);
    glm::mat4 deltaMat;

    ImGuiIO& io = ImGui::GetIO();
    ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
    ImGuizmo::SetDrawlist(ImGui::GetBackgroundDrawList());

    if ( ImGuizmo::Manipulate(  glm::value_ptr(camView),
                                glm::value_ptr(camProj),
                                mCurrentGizmoOperation,
                                mCurrentGizmoMode,
                                glm::value_ptr(nodeMat),
                                glm::value_ptr(deltaMat),
                                useSnap ? &snap[0] : nullptr,
                                nullptr,
                                nullptr ) )
    {
      nodeMat = node->toLocalSpace(nodeMat);

      glm::decompose( nodeMat,
                      nodeScale, nodeRotation, nodeTrans,
                      skew, pers );

      node->setOrigin(nodeTrans);
      node->setOrientation(nodeRotation);
      node->setScale(nodeScale);
    }
    ImGui::End();
  }

  ImGui::SetNextWindowPos({0.0f, 0.0f});
  ImGui::SetNextWindowSize({128.0f, 128.0f});
  ImGui::Begin("View", nullptr, ImGuiWindowFlags_NoMove);
  ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y,
                    ImGui::GetWindowWidth(), ImGui::GetWindowHeight());
  ImVec2 gizmoPos = {ImGui::GetWindowPos().x, ImGui::GetWindowPos().y};
  ImVec2 gizmoSize = { ImGui::GetWindowWidth(),  ImGui::GetWindowHeight()};
  ImGuizmo::SetDrawlist();

  ImGuizmo::ViewManipulate(glm::value_ptr(camView),
                           8.0f,
                           gizmoPos,
                           gizmoSize,
                           0x10101010);
  if ( ImGuizmo::IsUsing() )
    mCamera.setOrientation(glm::conjugate(glm::toQuat(camView)));
  ImGui::End();

  for ( auto drawable : mDepthBuffer )
    drawable.second->draw();
}
