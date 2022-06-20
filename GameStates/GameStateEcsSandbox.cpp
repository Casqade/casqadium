#include <GameStateEcsSandbox.hpp>
#include <GameStateController.hpp>

#include <TimeUtils/Duration.hpp>

#include <entt/entt.hpp>
#include <entt/core/hashed_string.hpp>

#include <cqde/alias.hpp>
#include <cqde/common.hpp>

#include <cqde/types/InputCallbackStorage.hpp>
#include <cqde/types/EntityTagStorage.hpp>

#include <cqde/types/assets/FontAssetManager.hpp>
#include <cqde/types/assets/TextureAssetManager.hpp>

#include <cqde/types/input/InputBinding.hpp>
#include <cqde/types/input/InputBindingAbsolute.hpp>
#include <cqde/types/input/InputBindingRelative.hpp>

#include <cqde/util/logger.hpp>

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
assignBinding(
  cqde::types::InputBindings& inputBindings,
  const cqde::InputAxisId axisId,
  const std::set <std::shared_ptr <cqde::types::InputBinding>,
    cqde::types::InputBindingComparator>& axisBindings )
{
  for ( auto& binding : axisBindings )
    inputBindings.insert({ binding, axisId });
}

void
initSwControls( cqde::types::InputBindings& bindings )
{
  using namespace cqde::types;

  using cqde::InputAxisId;
  using cqde::InputHwId;

  assignBinding( bindings, "TranslateX",
  {
    std::make_shared <InputBindingRelative> ( InputHwId("-Key_A"), +1.0f ),
    std::make_shared <InputBindingRelative> ( InputHwId("+Key_A"), -1.0f ),
    std::make_shared <InputBindingRelative> ( InputHwId("+Key_D"), +1.0f ),
    std::make_shared <InputBindingRelative> ( InputHwId("-Key_D"), -1.0f ),
  });

  assignBinding( bindings, "TranslateY",
  {
    std::make_shared <InputBindingRelative> ( InputHwId("-Key_Shift"), +1.0f ),
    std::make_shared <InputBindingRelative> ( InputHwId("+Key_Shift"), -1.0f ),
    std::make_shared <InputBindingRelative> ( InputHwId("+Key_Space"), +1.0f ),
    std::make_shared <InputBindingRelative> ( InputHwId("-Key_Space"), -1.0f ),
  });

  assignBinding( bindings, "TranslateZ",
  {
    std::make_shared <InputBindingRelative> ( InputHwId("-Key_S"), +1.0f ),
    std::make_shared <InputBindingRelative> ( InputHwId("+Key_S"), -1.0f ),
    std::make_shared <InputBindingRelative> ( InputHwId("+Key_W"), +1.0f ),
    std::make_shared <InputBindingRelative> ( InputHwId("-Key_W"), -1.0f ),
  });

  assignBinding( bindings, "Yaw",
  {
    std::make_shared <InputBindingRelative> ( InputHwId("+MouseMove_X"), -0.5f ),
    std::make_shared <InputBindingRelative> ( InputHwId("-MouseMove_X"), +0.5f ),
  });

  assignBinding( bindings, "Pitch",
  {
    std::make_shared <InputBindingRelative> ( InputHwId("+MouseMove_Y"), -0.5f ),
    std::make_shared <InputBindingRelative> ( InputHwId("-MouseMove_Y"), +0.5f ),
  });

  assignBinding( bindings, "CursorPosX",
  {
    std::make_shared <InputBindingAbsolute> ( InputHwId("+MousePos_X"), +1.0f ),
    std::make_shared <InputBindingAbsolute> ( InputHwId("-MousePos_X"), +1.0f ),
  });

  assignBinding( bindings, "CursorPosY",
  {
    std::make_shared <InputBindingAbsolute> ( InputHwId("+MousePos_Y"), +1.0f ),
    std::make_shared <InputBindingAbsolute> ( InputHwId("-MousePos_Y"), +1.0f ),
  });

  assignBinding( bindings, "CameraLookToggle",
  {
    std::make_shared <InputBindingRelative> ( InputHwId("+MouseButton_Middle"), +1.0f ),
    std::make_shared <InputBindingRelative> ( InputHwId("-MouseButton_Middle"), -1.0f ),
  });

  assignBinding( bindings, "EngineShutdown",
  {
    std::make_shared <InputBindingRelative> ( InputHwId("-Key_Enter"), +1.0f ),
  });
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

  std::cout << "registry.each\n";

  registry.each(
  [&registry] ( const entt::entity entity )
  {
    std::cout << "  entity " << entt::id_type(entity) << "\n";
    std::cout << "  components: " << "\n";

    each_component( entity, registry,
    [] ( const entt::id_type componentType, const entt::entity )
    {
      auto prop = entt::resolve(componentType).prop("typeName"_hs);

      std::cout << prop.value().cast <std::string> () << "\n";
    });

    std::cout << "\n";
  });

//  return;

  std::cout << "[comp, entities] registry storage\n";

  for ( auto [componentType, entities] : registry.storage() )
  {
    std::cout << "  each entity\n";
    for ( auto entity : entities )
      {
        auto type = entt::resolve(componentType);

        entt::meta_any any;
        if ( auto getFunc = type.func("Get"_hs) )
          any = getFunc.invoke(any, entt::forward_as_meta(registry), entity);

        if ( auto serialize = type.func("serialize"_hs) )
          std::cout << serialize.invoke(any).cast <Json::Value> ().toStyledString() << "\n";
      }
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

  std::cout << "comp names: " << comp1.name << " " << comp2.name << "\n";
}

std::shared_ptr <olc::Renderable>
textureFromText(
  const std::string& text,
  const olc::Pixel& color,
  const bool monospaced,
  olc::PixelGameEngine* pge )
{
  const auto textSize = monospaced ?
                          pge->GetTextSize(text)
                        : pge->GetTextSizeProp(text);

  auto texture = std::make_shared <olc::Renderable> ();
  texture->Create(textSize.x, textSize.y);

  const auto drawTargetPrev = pge->GetDrawTarget();

  pge->SetDrawTarget(texture->Sprite());
  monospaced ?
      pge->DrawString({0, 0}, text, color)
    : pge->DrawStringProp({0, 0}, text, color);
  pge->SetDrawTarget(drawTargetPrev);

  texture->Decal()->Update();

  return texture;
}

std::shared_ptr <olc::Renderable>
texturePlaceholder()
{
  auto texture = std::make_shared <olc::Renderable> ();
  texture->Create(16, 16);

  for ( int32_t px = 0;
        px < texture->Sprite()->width;
        ++px )
    for ( int32_t py = 0;
          py < texture->Sprite()->height;
          ++py )
    texture->Sprite()->SetPixel(px, py,
                                (px + py) % 2 == 0 ? olc::BLACK : olc::MAGENTA);

  texture->Decal()->Update();

  return texture;
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
  initSwControls( mRegistry.ctx().at <InputBindings> () );

//  testSerialization();
//  return;

  auto& fonts = mRegistry.ctx().at <FontAssetManager> ();
  auto& textures = mRegistry.ctx().at <TextureAssetManager> ();

  auto textureNull = texturePlaceholder();
  auto textureError = textureFromText("ERROR", olc::RED, true, mPGE);

  textures.insert("null", textureNull);
  textures.insert("error", textureError);

  try
  {
    fonts.parseFile("data/editor/fonts.json");
    fonts.load({"munro"});
  }
  catch ( const std::exception& e )
  {
    LOG_ERROR("{}", e.what());
    return;
  }

  try
  {
    textures.parseFile("data/editor/textures.json");
    textures.load({"map_diffuse"});
    textures.load({"map_height"});
  }
  catch ( const std::exception& e )
  {
    LOG_ERROR("{}", e.what());
    return;
  }

  while (fonts.status("munro") != AssetStatus::Loaded)
    ;

  auto textTexture = std::make_shared <olc::Renderable> (fonts.get("munro")->RenderStringToRenderable(U"T", olc::WHITE));
  textures.insert("text_texture", textTexture);

  auto eQuad = mRegistry.create();
  auto eCamera = mRegistry.create();

  mRegistry.emplace <Transform> (eQuad) = Transform({0.0f, 0.0f, -10.0f});
  mRegistry.emplace <SceneNode> (eQuad) = SceneNode();
  mRegistry.emplace <GeometryBuffer> (eQuad) = GeometryBuffer(defaultQuadVerts);
  auto& textureBuffer = mRegistry.emplace <TextureBuffer> (eQuad) = TextureBuffer();

  auto& cCamera = mRegistry.emplace <Camera> (eCamera) = Camera();
  mRegistry.emplace <Transform> (eCamera) = Transform();
  mRegistry.emplace <SceneNode> (eCamera) = SceneNode();

  cCamera.viewport = { 0.0f, 0.0f, mPGE->GetWindowSize().x, mPGE->GetWindowSize().y};

  textureBuffer.textures.push_back("map_diffuse");
  textureBuffer.textures.push_back("text_texture2");

  ControlAxis iAxisTranslateX{};
  iAxisTranslateX.constraint = {-1.0f, 1.0f};
  iAxisTranslateX.value = 0.0f;

  ControlAxis iAxisTranslateY{};
  iAxisTranslateY.constraint = {-1.0f, 1.0f};
  iAxisTranslateY.value = 0.0f;

  ControlAxis iAxisTranslateZ{};
  iAxisTranslateZ.constraint = {-1.0f, 1.0f};
  iAxisTranslateZ.value = 0.0f;

  ControlAxis iAxisCameraLookToggle{};
  iAxisCameraLookToggle.constraint = {0.0f, 1.0f};
  iAxisCameraLookToggle.value = 0.0f;
  iAxisCameraLookToggle.callbacks.insert(cqde::InputCallbackId("CameraLookToggle"));

  ControlAxis iAxisExitListener{};
  iAxisExitListener.constraint = {0.0f, 1.0f};
  iAxisExitListener.value = 0.0f;
  iAxisExitListener.callbacks.insert(cqde::InputCallbackId("QuitGame"));

  ControlAxis iAxisCursorListener{};
  iAxisCursorListener.constraint = {0, mPGE->GetWindowSize().x};
  iAxisCursorListener.value = mPGE->GetMouseX();

  auto& cInputController = mRegistry.emplace <InputController> (eCamera);
  cInputController.inputs[cqde::InputAxisId("TranslateX")] = iAxisTranslateX;
  cInputController.inputs[cqde::InputAxisId("TranslateY")] = iAxisTranslateY;
  cInputController.inputs[cqde::InputAxisId("TranslateZ")] = iAxisTranslateZ;
  cInputController.inputs[cqde::InputAxisId("CameraLookToggle")] = iAxisCameraLookToggle;
  cInputController.inputs[cqde::InputAxisId("EngineShutdown")] = iAxisExitListener;
  cInputController.inputs[cqde::InputAxisId("CursorPosX")] = iAxisCursorListener;

  auto& inputCallbacks = mRegistry.ctx().at <InputCallbackStorage> ();

  inputCallbacks.Register( cqde::InputCallbackId("CameraLookToggle"),
  [] ( const entt::entity, InputController& cController )
  {
    static float valPitch{};
    static float valYaw{};

    if ( cController.inputs.count("Pitch") > 0 || cController.inputs.count("Yaw") > 0 )
    {
      valPitch = cController.inputs["Pitch"].value;
      valYaw = cController.inputs["Yaw"].value;

      cController.inputs.erase("Pitch");
      cController.inputs.erase("Yaw");

      return;
    }

    ControlAxis iAxisPitch{};
    iAxisPitch.constraint = {-90.0f, 90.0f};
    iAxisPitch.value = valPitch;

    ControlAxis iAxisYaw{};
    iAxisYaw.constraint = {-3600.0f, 3600.0f};
    iAxisYaw.value = valYaw;
    iAxisYaw.callbacks.insert(cqde::InputCallbackId("CameraYawClamp"));

    cController.inputs["Pitch"] = iAxisPitch;
    cController.inputs["Yaw"] = iAxisYaw;

    return;
  });

  inputCallbacks.Register( cqde::InputCallbackId("CameraYawClamp"),
  [] ( const entt::entity, InputController& cController )
  {
    if ( cController.inputs.count("Yaw") == 0 )
      return;

    float& yaw = cController.inputs["Yaw"].value;
    yaw = yaw > 180.0f ? yaw - 360.0f : yaw;
    yaw = yaw < -180.0f ? yaw + 360.0f : yaw;
  });

  inputCallbacks.Register( cqde::InputCallbackId("QuitGame"),
  [this] ( const entt::entity, InputController& )
  {
    mRunning = false;
  });
}

void
GameStateEcsSandbox::handleAxisInput(
  const cqde::InputHwId inputSrc,
  const float amount,
  const float direction )
{
  using namespace cqde::compos;
  using namespace cqde::types;

  auto& bindings = mRegistry.ctx().at <InputBindings> ();

  const std::string inputDir  = direction > 0.0f
                              ? "+"
                              : "-";

  auto& inputCallbacks = mRegistry.ctx().at <InputCallbackStorage> ();

  const auto [axesBegin, axesEnd] = bindings.equal_range( inputDir + inputSrc.str() );

  for ( auto iter = axesBegin;
        iter != axesEnd;
        ++iter )
  {
    auto& [binding, axisId] = *iter;

    for ( auto&& [entity, cController] : mRegistry.view <InputController> ().each() )
      if ( cController.inputs.count(iter->second) != 0 )
      {
        binding->handleInput( cController.inputs[axisId], amount );

          for ( const auto& callbackId : cController.inputs[axisId].callbacks )
            inputCallbacks.Execute(callbackId, entity, cController);
      }
  }
}

void
GameStateEcsSandbox::keyEvent( const olc::Event event )
{
  using namespace cqde::compos;
  using namespace cqde::types;

  auto& hwControls = mRegistry.ctx().at <HwControlMap> ();

  const float inputDirection = event.type == olc::Event::KeyPressed
                          ? +1.0f
                          : -1.0f;

  handleAxisInput( hwControls[event.key.code], 1.0f, inputDirection );
}

void
GameStateEcsSandbox::mouseMoveEvent( const olc::Event::MouseMoveEvent event )
{
  using olc::MouseInputId;
  using namespace cqde::compos;
  using namespace cqde::types;

  auto& hwControls = mRegistry.ctx().at <HwControlMap> ();

  if ( event.dx != 0 )
  {
    handleAxisInput( hwControls[int32_t(MouseInputId::MoveX)], std::abs(event.dx), event.dx );
    handleAxisInput( hwControls[int32_t(MouseInputId::PosX)], mPGE->GetMouseX(), event.dx );
  }

  if ( event.dy != 0 )
  {
    handleAxisInput( hwControls[int32_t(MouseInputId::MoveY)], std::abs(event.dy), event.dy );
    handleAxisInput( hwControls[int32_t(MouseInputId::PosY)], mPGE->GetMouseY(), event.dy );
  }
}

void
GameStateEcsSandbox::mouseButtonEvent( const olc::Event event )
{
  using olc::MouseInputId;
  using namespace cqde::types;

  auto& hwControls = mRegistry.ctx().at <HwControlMap> ();

  const int32_t inputId = int32_t(MouseInputId::ENUM_BEGIN) + int32_t(event.mouseButton.button);

  const float inputDirection = event.type == olc::Event::MouseButtonPressed
                            ? 1.0f
                            : -1.0f;

  handleAxisInput( hwControls[inputId], 1.0f, inputDirection );
}

bool
GameStateEcsSandbox::update(  const uint32_t ticks,
                              const TimeUtils::Duration elapsed )
{
  using namespace cqde::compos;
  const double dt = static_cast <double> (elapsed);
  const float cameraVelocity = 10.0f;

// Camera control system
  for ( auto&& [eCamera, cCamera, cController, cTransform] : mRegistry.view <Camera, InputController, Transform>().each() )
  {
    const float translationX = cController.inputs[cqde::InputAxisId("TranslateX")].value * cameraVelocity * dt;
    cTransform.translation += cTransform.right() * translationX;

    const float translationY = cController.inputs[cqde::InputAxisId("TranslateY")].value * cameraVelocity * dt;
    cTransform.translation += cTransform.up() * translationY;

    const float translationZ = cController.inputs[cqde::InputAxisId("TranslateZ")].value * cameraVelocity * dt;
    cTransform.translation += cTransform.front() * translationZ;

    if ( cController.inputs.count("Pitch") == 0 || cController.inputs.count("Yaw") == 0 )
      continue;

    const float pitch = glm::radians( cController.inputs[cqde::InputAxisId("Pitch")].value );
    const float yaw = glm::radians( cController.inputs[cqde::InputAxisId("Yaw")].value );

    cTransform.orientation = glm::quat( {pitch, yaw, 0.0f} );
  }

  return mRunning;
}

void
GameStateEcsSandbox::render()
{
  cqde::systems::CullingSystem(mRegistry);
  cqde::systems::RenderSystem(mRegistry);
}
