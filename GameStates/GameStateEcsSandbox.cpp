#include <GameStateEcsSandbox.hpp>
#include <GameStateController.hpp>

#include <TimeUtils/Duration.hpp>

#include <entt/entt.hpp>
#include <entt/core/hashed_string.hpp>

#include <cqde/alias.hpp>
#include <cqde/common.hpp>
#include <cqde/types/InputCallbackStorage.hpp>
#include <cqde/types/EntityTagStorage.hpp>
#include <cqde/components/Camera.hpp>
#include <cqde/components/SceneNode.hpp>
#include <cqde/components/Transform.hpp>
#include <cqde/components/GeometryBuffer.hpp>
#include <cqde/components/InputController.hpp>
#include <cqde/components/TextureBuffer.hpp>

#include <cqde/systems/RenderSystem.hpp>
#include <cqde/systems/CullingSystem.hpp>

#include <olcPGE/olcMouseInputId.hpp>

#include <json/value.h>


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

template<typename Component>
decltype(auto) get(
  entt::registry& registry,
  entt::entity entity)
{
  std::cout << "got\n";
  return registry.template get <Component> (entity);
}

struct MyComponent
{
  std::string name;


  Json::Value serialize() const
  {
    std::cout << "serialize " << name << "\n";

    Json::Value component;
    component["name"] = name;

    return component;
  }

  static void deserialize( entt::registry& registry, entt::entity entity, const Json::Value& component )
  {
    auto& comp = registry.emplace_or_replace <MyComponent> (entity);
    comp.name = component.get("name", "defName").asString();

    std::cout << "deserialized " << comp.name << "\n";
  }
};

template <typename Func>
void each_component(  const entt::entity entity,
                      const entt::registry& registry,
                      Func func )
{
  for ( const auto [componentType, entities] : registry.storage() )
    if ( const auto iter = entities.find(entity); iter != entities.end() )
      func( componentType, *iter );
}

void
testSerialization()
{
  using namespace entt::literals;

  entt::registry registry {};


  entt::meta<MyComponent>().type("MyComponent"_hs)
    .props(std::make_pair("typeName"_hs, std::string("MyComponent")))
    .func<&MyComponent::serialize>("serialize"_hs)
    .func<&MyComponent::deserialize>("deserialize"_hs)
    .func<&get <MyComponent>>("Get"_hs);

  auto entity1 = registry.create();
  auto entity2 = registry.create();
  auto& comp1 = registry.emplace <MyComponent> (entity1);
  auto& comp2 = registry.emplace <MyComponent> (entity2);
  comp1.name = "test1";
  comp2.name = "test2";

  registry.each(
  [&registry] ( const entt::entity entity )
  {
    std::cout << "entity " << entt::id_type(entity) << "\n";
    std::cout << "components: " << "\n";

    each_component( entity, registry,
    [&registry] ( const entt::id_type componentType, const entt::entity entity )
    {
      auto prop = entt::resolve(componentType).prop("typeName"_hs);

      std::cout << prop.value().cast <std::string> () << "\n";
    });

    std::cout << "\n";
  });

  return;

  for ( auto [componentType, entities] : registry.storage() )
    for ( auto entity : entities )
    {
      auto type = entt::resolve(componentType);

      entt::meta_any any;
      if ( auto getFunc = type.func("Get"_hs) )
        any = getFunc.invoke(any, entt::forward_as_meta(registry), entity);

      if ( auto serialize = type.func("serialize"_hs) )
        std::cout << serialize.invoke(any).cast <Json::Value> ().toStyledString() << "\n";
    }

  std::cout << "comp names: " << comp1.name << " " << comp2.name << "\n";

  auto prop = entt::resolve("MyComponent"_hs).prop("typeName"_hs);
  Json::Value components;
  components[prop.value().cast <std::string> ()]["name"] = "serializedName";

  auto type = entt::resolve(entt::hashed_string(std::string(components.getMemberNames().front()).c_str()));

  entt::meta_any any;
  if ( auto getFunc = type.func("Get"_hs) )
    any = getFunc.invoke(any, entt::forward_as_meta(registry), entity1);

  if ( auto deserialize = type.func("deserialize"_hs) )
    deserialize.invoke(any, entt::forward_as_meta(registry), entity1, entt::forward_as_meta(components["MyComponent"]));

  std::cout << "comp names: " << registry.get <MyComponent> (entity1).name << " " << registry.get <MyComponent> (entity2).name << "\n";
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

  cqde::engineInit(mRegistry);
  initSwControls( mRegistry.ctx().at <SwControlMap> () );

  testSerialization();
  return;

  auto texture = std::make_shared <olc::Renderable> ();

  auto text = "T";
  const olc::vi2d textSize = mPGE->GetTextSize(text);
  texture->Create( textSize.x, textSize.y );

  mPGE->SetDrawTarget( texture->Sprite() );
  mPGE->Clear( olc::BLANK );
  mPGE->DrawString( 0, 0, text, olc::WHITE );
  mPGE->SetDrawTarget(nullptr);

  texture->Decal()->Update();

  auto& textures = mRegistry.ctx().at <TextureStorage> ();
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

  auto& inputCallbacks = mRegistry.ctx().at <InputCallbackStorage> ();

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

    auto& inputCallbacks = mRegistry.ctx().at <InputCallbackStorage> ();
    for ( const cqde::InputCallbackId callbackId : iAxis.callbacks )
      inputCallbacks.Execute( callbackId, entity, cInputController );
  }
}

void
GameStateEcsSandbox::keyEvent( const olc::Event event )
{
  using namespace cqde::compos;
  using namespace cqde::types;

  auto& hwControls = mRegistry.ctx().at <HwControlMap> ();
  auto& swControls = mRegistry.ctx().at <SwControlMap> ();

  const cqde::InputSourceId inputSrc = hwControls[event.key.code];

  assert( swControls.count(inputSrc) != 0 );

  if ( swControls.count(inputSrc) == 0 )
    return;

  std::string inputDstStr = swControls[inputSrc].str();

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

  auto& hwControls = mRegistry.ctx().at <HwControlMap> ();
  auto& swControls = mRegistry.ctx().at <SwControlMap> ();

  const cqde::InputSourceId inputSrc = hwControls[inputId];

  assert( swControls.count(inputSrc) != 0 );

  if ( swControls.count(inputSrc) == 0 )
    return;

  std::string inputDstStr = swControls[inputSrc].str();

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
