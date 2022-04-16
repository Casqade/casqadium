#include <GameStateEcsSandbox.hpp>
#include <GameStateController.hpp>

#include <TimeUtils/Duration.hpp>

#include <entt/entt.hpp>
#include <entt/core/hashed_string.hpp>

#include <ECS/Alias.hpp>
#include <ECS/Common.hpp>
#include <ECS/Types/CallbackStorage.hpp>
#include <ECS/Components/Camera.hpp>
#include <ECS/Components/SceneNode.hpp>
#include <ECS/Components/Transform.hpp>
#include <ECS/Components/GeometryBuffer.hpp>
#include <ECS/Components/InputController.hpp>
#include <ECS/Components/TextureBuffer.hpp>
#include <ECS/Components/TextureStorage.hpp>

#include <ECS/Systems/RenderSystem.hpp>
#include <ECS/Systems/CullingSystem.hpp>


void callbackTest( entt::entity, ECS::Components::InputController& )
{
  std::cout << "hello\n";
}


class InputCallbackStorage : public ECS::Types::CallbackStorage <std::function <void(const entt::entity, ECS::Components::InputController&)>>
{
public:
  void Execute( const ECS::CallbackId,
                const entt::entity,
                ECS::Components::InputController& );
};

void
InputCallbackStorage::Execute(
  const ECS::CallbackId callbackId,
  const entt::entity entity,
  ECS::Components::InputController& controller )
{
  mCallbacks[callbackId](entity, controller);
}

InputCallbackStorage inputCallbackStorage{};


enum class MouseInput
{
  ENUM_BEGIN = olc::Key::ENUM_END,
  ButtonLeft = ENUM_BEGIN,
  ButtonRight,
  ButtonMiddle,
  ButtonX1,
  ButtonX2,

  MoveX,
  MoveY,

  Wheel
};

std::map <int32_t, ECS::InputSourceId> keyMap
{
//  {olc::Key::NONE, ECS::InputSourceId("Key_None")},
  {olc::Key::A, ECS::InputSourceId("Key_A")},
  {olc::Key::D, ECS::InputSourceId("Key_D")},
  {olc::Key::S, ECS::InputSourceId("Key_S")},
  {olc::Key::W, ECS::InputSourceId("Key_W")},
  {olc::Key::SHIFT, ECS::InputSourceId("Key_Shift")},
  {olc::Key::SPACE, ECS::InputSourceId("Key_Space")},

  {int32_t(MouseInput::ButtonLeft), ECS::InputSourceId("MouseButton_Left")},
  {int32_t(MouseInput::ButtonRight), ECS::InputSourceId("MouseButton_Right")},
  {int32_t(MouseInput::ButtonMiddle), ECS::InputSourceId("MouseButton_Middle")},
  {int32_t(MouseInput::ButtonX1), ECS::InputSourceId("MouseButton_X1")},
  {int32_t(MouseInput::ButtonX2), ECS::InputSourceId("MouseButton_X2")},
  {int32_t(MouseInput::MoveX), ECS::InputSourceId("Mouse_MoveX")},
  {int32_t(MouseInput::MoveY), ECS::InputSourceId("Mouse_MoveY")},
  {int32_t(MouseInput::Wheel), ECS::InputSourceId("Wheel_Move")},
};

std::map <ECS::InputSourceId, ECS::InputDestinationId,
          entt::hashed_string_comparator> controlMap
{
  {ECS::InputSourceId("Key_A"), ECS::InputDestinationId("-TranslateX")},
  {ECS::InputSourceId("Key_D"), ECS::InputDestinationId("+TranslateX")},
  {ECS::InputSourceId("Key_S"), ECS::InputDestinationId("-TranslateZ")},
  {ECS::InputSourceId("Key_W"), ECS::InputDestinationId("+TranslateZ")},
  {ECS::InputSourceId("Key_Space"), ECS::InputDestinationId("+TranslateY")},
  {ECS::InputSourceId("Key_Shift"), ECS::InputDestinationId("-TranslateY")},
};


GameStateEcsSandbox::GameStateEcsSandbox( GameStateController* const stateController )
  : GameState(stateController)
  , mState(StateLocal::Idle)
{
  using namespace entt::literals;
  using namespace ECS::Components;

  ECS::registryInit(mRegistry);

  const olc::vi2d textSize = mPGE->GetTextSize( "T" );

  olc::Renderable texture = olc::Renderable();
  texture.Create( textSize.x, textSize.y );

  mPGE->SetDrawTarget( texture.Sprite() );
  mPGE->Clear( olc::BLANK );
  mPGE->DrawString( 0, 0, "T", olc::WHITE );
  mPGE->SetDrawTarget(nullptr);

  texture.Decal()->Update();

  ECS::AssetStorage = mRegistry.create();
  auto& textureStorage = mRegistry.emplace <TextureStorage> (ECS::AssetStorage);
  textureStorage.textures.emplace("test"_hs, std::move(texture));


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
//  textureBuffer.textures[1] = "test"_hs;

  ECS::Types::InputAxis iAxisTranslateX{};
  iAxisTranslateX.sensitivity = 1.0f;
  iAxisTranslateX.constraint = {-1.0f, 1.0f};
  iAxisTranslateX.value = 0.0f;

  ECS::Types::InputAxis iAxisTranslateY{};
  iAxisTranslateY.sensitivity = 1.0f;
  iAxisTranslateY.constraint = {-1.0f, 1.0f};
  iAxisTranslateY.value = 0.0f;
  iAxisTranslateY.callbacks.insert(ECS::InputCallbackId("CameraMoveUpward"));

  ECS::Types::InputAxis iAxisTranslateZ{};
  iAxisTranslateZ.sensitivity = 1.0f;
  iAxisTranslateZ.constraint = {-1.0f, 1.0f};
  iAxisTranslateZ.value = 0.0f;

  auto& cInputController = mRegistry.emplace <InputController> (eCamera);
  cInputController.inputs[ECS::InputDestinationId("TranslateX")] = iAxisTranslateX;
  cInputController.inputs[ECS::InputDestinationId("TranslateY")] = iAxisTranslateY;
  cInputController.inputs[ECS::InputDestinationId("TranslateZ")] = iAxisTranslateZ;

  inputCallbackStorage.Register( ECS::InputCallbackId("CameraMoveUpward"),
  [this] ( const entt::entity entity, ECS::Components::InputController& cController )
  {
    std::cout << "CameraMoveUpward callback\n";
  });
}

void
GameStateEcsSandbox::handleAxisInput(
  const float amount,
  const float direction,
  const std::string& inputDestinationId )
{
  using namespace ECS::Components;

  const ECS::InputDestinationId inputDst( inputDestinationId.c_str() );

  for ( auto&& [entity, cInputController] : mRegistry.view <InputController>().each() )
  {
    if ( cInputController.inputs.count(inputDst) == 0 )
      continue;

    ECS::Types::InputAxis& iAxis = cInputController.inputs[inputDst];

    iAxis.value = std::clamp(iAxis.value + iAxis.sensitivity * amount * direction,
                             iAxis.constraint.first,
                             iAxis.constraint.second );

    for ( const ECS::InputCallbackId callbackId : iAxis.callbacks )
      inputCallbackStorage.Execute( callbackId, entity, cInputController );
  }
}

void
GameStateEcsSandbox::keyEvent( const olc::Event event )
{
  using namespace ECS::Components;

  const ECS::InputSourceId inputSrc = keyMap[event.key.code];

//  assert( controlMap.count(inputSrc) != 0 );

  if ( controlMap.count(inputSrc) == 0 )
    return;

  std::string inputDstStr( controlMap[inputSrc].data() );

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
  using namespace ECS::Components;

  const ECS::InputSourceId inputSrc = keyMap[inputId];

//  assert( controlMap.count(inputSrc) != 0 );

  if ( controlMap.count(inputSrc) == 0 )
    return;

  std::string inputDstStr( controlMap[inputSrc].data() );

  const float controlDir = inputDstStr[0] == '-' ? -1.0f : 1.0f;

  if ( inputDstStr[0] == '+' || inputDstStr[0] == '-' )
    inputDstStr.erase( 0, 1 );

  handleAxisInput( inputAmount, controlDir, inputDstStr );
}

void
GameStateEcsSandbox::mouseMoveEvent( const olc::Event::MouseMoveEvent event )
{
  using namespace ECS::Components;

  if ( event.dx != 0 )
    handleMouseInput( int32_t(MouseInput::MoveX), event.dx );

  if ( event.dy != 0 )
    handleMouseInput( int32_t(MouseInput::MoveY), event.dy );
}

void
GameStateEcsSandbox::mouseButtonEvent( const olc::Event event )
{
  const float inputAmount = event.type == olc::Event::MouseButtonPressed
                          ? 1.0f
                          : -1.0f;

  handleMouseInput( int32_t(MouseInput::ENUM_BEGIN) + int32_t(event.mouseButton.button), inputAmount );
}

bool
GameStateEcsSandbox::update(  const uint32_t ticks,
                              const TimeUtils::Duration elapsed )
{
  using namespace ECS::Components;

  static uint32_t ticks_total = 0;

  const double dt = static_cast <double> (elapsed);
  const float cameraVelocity = 10.0f;

  for ( auto&& [eCamera, cCamera, cController, cTransform] : mRegistry.view <Camera, InputController, Transform>().each() )
  {
    const float translationX = cController.inputs[ECS::InputDestinationId("TranslateX")].value * cameraVelocity * dt;
    cTransform.translation += cTransform.right() * translationX;

    const float translationY = cController.inputs[ECS::InputDestinationId("TranslateY")].value * cameraVelocity * dt;
    cTransform.translation += cTransform.up() * translationY;

    const float translationZ = cController.inputs[ECS::InputDestinationId("TranslateZ")].value * cameraVelocity * dt;
    cTransform.translation += cTransform.front() * translationZ;
  }

  return (ticks_total += ticks) < 6000;
}

void
GameStateEcsSandbox::render()
{
  ECS::Systems::CullSystem(mRegistry);
  ECS::Systems::RenderSystem(mRegistry);
}
