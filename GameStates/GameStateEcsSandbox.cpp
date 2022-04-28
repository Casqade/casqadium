#include <GameStateEcsSandbox.hpp>
#include <GameStateController.hpp>

#include <TimeUtils/Duration.hpp>

#include <entt/entt.hpp>
#include <entt/core/hashed_string.hpp>

#include <cqde/alias.hpp>
#include <cqde/common.hpp>
#include <cqde/types/InputCallbackStorage.hpp>
#include <cqde/components/Camera.hpp>
#include <cqde/components/SceneNode.hpp>
#include <cqde/components/Transform.hpp>
#include <cqde/components/GeometryBuffer.hpp>
#include <cqde/components/InputController.hpp>
#include <cqde/components/TextureBuffer.hpp>

#include <cqde/systems/RenderSystem.hpp>
#include <cqde/systems/CullingSystem.hpp>

#include <olcPGE/olcMouseInputId.hpp>


void
initSwControls( cqde::types::SwControlMap& controlMap )
{
  controlMap =
  {
    {cqde::InputSourceId("Key_A"), cqde::InputDestinationId("-TranslateX")},
    {cqde::InputSourceId("Key_D"), cqde::InputDestinationId("+TranslateX")},
    {cqde::InputSourceId("Key_S"), cqde::InputDestinationId("-TranslateZ")},
    {cqde::InputSourceId("Key_W"), cqde::InputDestinationId("+TranslateZ")},
    {cqde::InputSourceId("Key_Space"), cqde::InputDestinationId("+TranslateY")},
    {cqde::InputSourceId("Key_Shift"), cqde::InputDestinationId("-TranslateY")},

    {cqde::InputSourceId("Key_Enter"), cqde::InputDestinationId("Quit")},

    {cqde::InputSourceId("Mouse_MoveX"), cqde::InputDestinationId("-Yaw")},
    {cqde::InputSourceId("Mouse_MoveY"), cqde::InputDestinationId("-Pitch")},

    {cqde::InputSourceId("MouseButton_Middle"), cqde::InputDestinationId("CameraLookToggle")},
  };
}


GameStateEcsSandbox::GameStateEcsSandbox( GameStateController* const stateController )
  : GameState(stateController)
  , mState(StateLocal::Idle)
  , mRegistry()
  , mRunning(true)
{
  using namespace entt::literals;
  using namespace cqde::compos;
  using namespace cqde::types;

  cqde::registryInit(mRegistry);
  initSwControls( mRegistry.ctx <SwControlMap> () );

  auto texture = std::make_shared <olc::Renderable> ();

  auto text = "T";
  const olc::vi2d textSize = mPGE->GetTextSize(text);
  texture->Create( textSize.x, textSize.y );

  mPGE->SetDrawTarget( texture->Sprite() );
  mPGE->Clear( olc::BLANK );
  mPGE->DrawString( 0, 0, text, olc::WHITE );
  mPGE->SetDrawTarget(nullptr);

  texture->Decal()->Update();

  auto& textures = mRegistry.ctx <TextureStorage> ();
  textures.emplace("test"_hs, texture );

  auto eQuad = mRegistry.create();
  auto eCamera = mRegistry.create();

  mRegistry.emplace <Transform> (eQuad) = Transform({0.0f, 0.0f, -10.0f});
  mRegistry.emplace <SceneNode> (eQuad) = SceneNode();
  mRegistry.emplace <GeometryBuffer> (eQuad) = GeometryBuffer(defaultQuadVerts);
  auto& textureBuffer = mRegistry.emplace <TextureBuffer> (eQuad) = TextureBuffer();

  auto& cCamera = mRegistry.emplace <Camera> (eCamera) = Camera();
  mRegistry.emplace <Transform> (eCamera) = Transform();
  mRegistry.emplace <SceneNode> (eCamera) = SceneNode();

  cCamera.viewport = { 0.0f, 0.0f, mPGE->GetWindowSize().x, mPGE->GetWindowSize().y };

  textureBuffer.textures[0] = "test"_hs;

  InputAxis iAxisTranslateX{};
  iAxisTranslateX.sensitivity = 1.0f;
  iAxisTranslateX.constraint = {-1.0f, 1.0f};
  iAxisTranslateX.value = 0.0f;

  InputAxis iAxisTranslateY{};
  iAxisTranslateY.sensitivity = 1.0f;
  iAxisTranslateY.constraint = {-1.0f, 1.0f};
  iAxisTranslateY.value = 0.0f;

  InputAxis iAxisTranslateZ{};
  iAxisTranslateZ.sensitivity = 1.0f;
  iAxisTranslateZ.constraint = {-1.0f, 1.0f};
  iAxisTranslateZ.value = 0.0f;

  InputAxis iAxisPitch{};
  iAxisPitch.sensitivity = 0.0f;
  iAxisPitch.constraint = {-90.0f, 90.0f};
  iAxisPitch.value = 0.0f;

  InputAxis iAxisYaw{};
  iAxisYaw.sensitivity = 0.0f;
  iAxisYaw.constraint = {-3600.0f, 3600.0f};
  iAxisYaw.value = 0.0f;
  iAxisYaw.callbacks.insert(cqde::InputCallbackId("CameraYawClamp"));

  InputAxis iAxisCameraLookToggle{};
  iAxisCameraLookToggle.sensitivity = 1.0f;
  iAxisCameraLookToggle.constraint = {0.0f, 1.0f};
  iAxisCameraLookToggle.value = 0.0f;
  iAxisCameraLookToggle.callbacks.insert(cqde::InputCallbackId("CameraLookToggle"));

  InputAxis iAxisExitListener{};
  iAxisExitListener.sensitivity = 1.0f;
  iAxisExitListener.constraint = {0.0f, 1.0f};
  iAxisExitListener.value = 0.0f;
  iAxisExitListener.callbacks.insert(cqde::InputCallbackId("QuitGame"));

  auto& cInputController = mRegistry.emplace <InputController> (eCamera);
  cInputController.inputs[cqde::InputDestinationId("TranslateX")] = iAxisTranslateX;
  cInputController.inputs[cqde::InputDestinationId("TranslateY")] = iAxisTranslateY;
  cInputController.inputs[cqde::InputDestinationId("TranslateZ")] = iAxisTranslateZ;
  cInputController.inputs[cqde::InputDestinationId("Pitch")] = iAxisPitch;
  cInputController.inputs[cqde::InputDestinationId("Yaw")] = iAxisYaw;
  cInputController.inputs[cqde::InputDestinationId("CameraLookToggle")] = iAxisCameraLookToggle;
  cInputController.inputs[cqde::InputDestinationId("Quit")] = iAxisExitListener;

  auto& inputCallbacks = mRegistry.ctx <InputCallbackStorage> ();

  inputCallbacks.Register( cqde::InputCallbackId("CameraLookToggle"),
  [this] ( const entt::entity entity, InputController& cController )
  {
    InputAxis& pitch = cController.inputs["Pitch"];
    InputAxis& yaw = cController.inputs["Yaw"];

    if ( pitch.sensitivity > 0.0f )
      pitch.sensitivity = 0.0f;
    else
      pitch.sensitivity = 0.5f;

    if ( yaw.sensitivity > 0.0f )
      yaw.sensitivity = 0.0f;
    else
      yaw.sensitivity = 0.5f;
  });

  inputCallbacks.Register( cqde::InputCallbackId("CameraYawClamp"),
  [this] ( const entt::entity entity, InputController& cController )
  {
    float& yaw = cController.inputs["Yaw"].value;
    yaw = yaw > 180.0f ? yaw - 360.0f : yaw;
    yaw = yaw < -180.0f ? yaw + 360.0f : yaw;
  });

  inputCallbacks.Register( cqde::InputCallbackId("QuitGame"),
  [this] ( const entt::entity entity, InputController& cController )
  {
    mRunning = false;
  });
}

void
GameStateEcsSandbox::handleAxisInput(
  const float amount,
  const float direction,
  const std::string& inputDestinationId )
{
  using namespace cqde::compos;
  using namespace cqde::types;

  const cqde::InputDestinationId inputDst( inputDestinationId.c_str() );

  for ( auto&& [entity, cInputController] : mRegistry.view <InputController>().each() )
  {
    if ( cInputController.inputs.count(inputDst) == 0 )
      continue;

    InputAxis& iAxis = cInputController.inputs[inputDst];

    iAxis.value = std::clamp(iAxis.value + iAxis.sensitivity * amount * direction,
                             iAxis.constraint.first,
                             iAxis.constraint.second );

    auto& inputCallbacks = mRegistry.ctx <InputCallbackStorage> ();
    for ( const cqde::InputCallbackId callbackId : iAxis.callbacks )
      inputCallbacks.Execute( callbackId, entity, cInputController );
  }
}

void
GameStateEcsSandbox::keyEvent( const olc::Event event )
{
  using namespace cqde::compos;
  using namespace cqde::types;

  auto& hwControls = mRegistry.ctx <HwControlMap> ();
  auto& SwControls = mRegistry.ctx <SwControlMap> ();

  const cqde::InputSourceId inputSrc = hwControls[event.key.code];

  assert( SwControls.count(inputSrc) != 0 );

  if ( SwControls.count(inputSrc) == 0 )
    return;

  std::string inputDstStr( SwControls[inputSrc].data() );

  const float inputAmount = event.type == olc::Event::KeyPressed ? 1.0f : -1.0f;
  const float controlDir = inputDstStr[0] == '-' ? -1.0f : 1.0f;

  if ( inputDstStr[0] == '+' || inputDstStr[0] == '-' )
    inputDstStr.erase( 0, 1 );

  handleAxisInput( inputAmount, controlDir, inputDstStr );
}

void
GameStateEcsSandbox::handleMouseInput(
  const int32_t inputId,
  const float inputAmount )
{
  using namespace cqde::compos;
  using namespace cqde::types;

  auto& hwControls = mRegistry.ctx <HwControlMap> ();
  auto& swControls = mRegistry.ctx <SwControlMap> ();

  const cqde::InputSourceId inputSrc = hwControls[inputId];

  assert( swControls.count(inputSrc) != 0 );

  if ( swControls.count(inputSrc) == 0 )
    return;

  std::string inputDstStr( swControls[inputSrc].data() );

  const float controlDir = inputDstStr[0] == '-' ? -1.0f : 1.0f;

  if ( inputDstStr[0] == '+' || inputDstStr[0] == '-' )
    inputDstStr.erase( 0, 1 );

  handleAxisInput( inputAmount, controlDir, inputDstStr );
}

void
GameStateEcsSandbox::mouseMoveEvent( const olc::Event::MouseMoveEvent event )
{
  using olc::MouseInputId;
  using namespace cqde::compos;

  if ( event.dx != 0 )
    handleMouseInput( int32_t(MouseInputId::MoveX), event.dx );

  if ( event.dy != 0 )
    handleMouseInput( int32_t(MouseInputId::MoveY), event.dy );
}

void
GameStateEcsSandbox::mouseButtonEvent( const olc::Event event )
{
  using olc::MouseInputId;

  const float inputAmount = event.type == olc::Event::MouseButtonPressed
                          ? 1.0f
                          : -1.0f;

  handleMouseInput( int32_t(MouseInputId::ENUM_BEGIN) + int32_t(event.mouseButton.button), inputAmount );
}

bool
GameStateEcsSandbox::update(  const uint32_t ticks,
                              const TimeUtils::Duration elapsed )
{
  using namespace cqde::compos;

  static uint32_t ticks_total = 0;

  const double dt = static_cast <double> (elapsed);
  const float cameraVelocity = 10.0f;

// Camera control system
  for ( auto&& [eCamera, cCamera, cController, cTransform] : mRegistry.view <Camera, InputController, Transform>().each() )
  {
    const float translationX = cController.inputs[cqde::InputDestinationId("TranslateX")].value * cameraVelocity * dt;
    cTransform.translation += cTransform.right() * translationX;

    const float translationY = cController.inputs[cqde::InputDestinationId("TranslateY")].value * cameraVelocity * dt;
    cTransform.translation += cTransform.up() * translationY;

    const float translationZ = cController.inputs[cqde::InputDestinationId("TranslateZ")].value * cameraVelocity * dt;
    cTransform.translation += cTransform.front() * translationZ;

    const float pitch = glm::radians( cController.inputs[cqde::InputDestinationId("Pitch")].value );
    const float yaw = glm::radians( cController.inputs[cqde::InputDestinationId("Yaw")].value );

    cTransform.orientation = glm::quat( {pitch, yaw, 0.0f} );
  }

  return mRunning && (ticks_total += ticks) < 6000;
}

void
GameStateEcsSandbox::render()
{
  cqde::systems::CullingSystem(mRegistry);
  cqde::systems::RenderSystem(mRegistry);
}
