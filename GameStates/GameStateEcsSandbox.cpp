#include <GameStateEcsSandbox.hpp>
#include <GameStateController.hpp>

#include <TimeUtils/Duration.hpp>

#include <entt/entt.hpp>
#include <entt/core/hashed_string.hpp>

#include <cqde/alias.hpp>
#include <cqde/common.hpp>

#include <cqde/types/EntityTagManager.hpp>
#include <cqde/types/PackageManager.hpp>

#include <cqde/types/assets/FontAssetManager.hpp>
#include <cqde/types/assets/TextureAssetManager.hpp>
#include <cqde/types/assets/TextStringAssetManager.hpp>

#include <cqde/types/input/InputManager.hpp>
#include <cqde/types/input/InputBinding.hpp>
#include <cqde/types/input/InputBindingAbsolute.hpp>
#include <cqde/types/input/InputBindingRelative.hpp>
#include <cqde/types/input/InputCallbackStorage.hpp>

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


static void
initSwControls( cqde::types::InputManager& inputManager )
{
  using namespace cqde::types;

  using cqde::InputAxisId;
  using cqde::InputHwId;

  inputManager.assignBindings("TranslateX",
  {
    std::make_shared <InputBindingRelative> ( InputHwId("-Key_A"), +1.0f ),
    std::make_shared <InputBindingRelative> ( InputHwId("+Key_A"), -1.0f ),
    std::make_shared <InputBindingRelative> ( InputHwId("+Key_D"), +1.0f ),
    std::make_shared <InputBindingRelative> ( InputHwId("-Key_D"), -1.0f ),
  });

  inputManager.assignBindings("TranslateY",
  {
    std::make_shared <InputBindingRelative> ( InputHwId("-Key_Shift"), +1.0f ),
    std::make_shared <InputBindingRelative> ( InputHwId("+Key_Shift"), -1.0f ),
    std::make_shared <InputBindingRelative> ( InputHwId("+Key_Space"), +1.0f ),
    std::make_shared <InputBindingRelative> ( InputHwId("-Key_Space"), -1.0f ),
  });

  inputManager.assignBindings("TranslateZ",
  {
    std::make_shared <InputBindingRelative> ( InputHwId("-Key_S"), +1.0f ),
    std::make_shared <InputBindingRelative> ( InputHwId("+Key_S"), -1.0f ),
    std::make_shared <InputBindingRelative> ( InputHwId("+Key_W"), +1.0f ),
    std::make_shared <InputBindingRelative> ( InputHwId("-Key_W"), -1.0f ),
  });

  inputManager.assignBindings("Yaw",
  {
    std::make_shared <InputBindingRelative> ( InputHwId("+MouseMove_X"), -0.5f ),
    std::make_shared <InputBindingRelative> ( InputHwId("-MouseMove_X"), +0.5f ),
  });

  inputManager.assignBindings("Pitch",
  {
    std::make_shared <InputBindingRelative> ( InputHwId("+MouseMove_Y"), -0.5f ),
    std::make_shared <InputBindingRelative> ( InputHwId("-MouseMove_Y"), +0.5f ),
  });

  inputManager.assignBindings("CursorPosX",
  {
    std::make_shared <InputBindingAbsolute> ( InputHwId("+MousePos_X"), +1.0f ),
    std::make_shared <InputBindingAbsolute> ( InputHwId("-MousePos_X"), +1.0f ),
  });

  inputManager.assignBindings("CursorPosY",
  {
    std::make_shared <InputBindingAbsolute> ( InputHwId("+MousePos_Y"), +1.0f ),
    std::make_shared <InputBindingAbsolute> ( InputHwId("-MousePos_Y"), +1.0f ),
  });

  inputManager.assignBindings("CameraLookOn",
  {
    std::make_shared <InputBindingRelative> ( InputHwId("+Key_Alt"), +1.0f ),
  });

  inputManager.assignBindings("CameraLookOff",
  {
    std::make_shared <InputBindingRelative> ( InputHwId("-Key_Alt"), +1.0f ),
  });

  inputManager.assignBindings("CameraLookToggle",
  {
    std::make_shared <InputBindingRelative> ( InputHwId("-Key_C"), +1.0f ),
  });

  inputManager.assignBindings("EngineShutdown",
  {
    std::make_shared <InputBindingRelative> ( InputHwId("-Key_Enter"), +1.0f ),
  });
}

template<typename Component>
static decltype(auto) get(
  entt::registry& registry,
  entt::entity entity)
{
  LOG_INFO("got");
  return registry.template get <Component> (entity);
}

struct MyComponent
{
  std::string name;


  Json::Value serialize() const
  {
    LOG_INFO("serializing {}", name);

    Json::Value component;
    component["name"] = name;

    return component;
  }

  static void deserialize( entt::registry& registry, entt::entity entity, const Json::Value& component )
  {
    auto& comp = registry.emplace_or_replace <MyComponent> (entity);
    comp.name = component.get("name", "defName").asString();

    LOG_INFO("deserialized {}", comp.name);
  }
};

template <typename Func>
static void each_component( const entt::entity entity,
                            const entt::registry& registry,
                            Func func )
{
  for ( const auto [componentType, entities] : registry.storage() )
    if ( const auto iter = entities.find(entity); iter != entities.end() )
      func( componentType, *iter );
}

static void
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

  LOG_INFO("registry.each");

  registry.each(
  [&registry] ( const entt::entity entity )
  {
    LOG_INFO("entity {}", entt::id_type(entity));
    LOG_INFO("components:");

    each_component( entity, registry,
    [] ( const entt::id_type componentType, const entt::entity )
    {
      auto prop = entt::resolve(componentType).prop("typeName"_hs);

      LOG_INFO("{}", prop.value().cast <std::string> ());
    });
    LOG_INFO("");
  });

//  return;

  LOG_INFO("[comp, entities] registry storage");

  for ( auto [componentType, entities] : registry.storage() )
  {
    LOG_INFO("each entity");
    for ( auto entity : entities )
      {
        auto type = entt::resolve(componentType);

        entt::meta_any any;
        if ( auto getFunc = type.func("Get"_hs) )
          any = getFunc.invoke(any, entt::forward_as_meta(registry), entity);

        if ( auto serialize = type.func("serialize"_hs) )
          LOG_INFO("{}", serialize.invoke(any).cast <Json::Value> ().toStyledString());
      }
  }

  LOG_INFO("comp names before: {} {}", comp1.name, comp2.name);

  auto prop = entt::resolve("MyComponent"_hs).prop("typeName"_hs);
  Json::Value components;
  components[prop.value().cast <std::string> ()]["name"] = "serializedName";

  auto type = entt::resolve(entt::hashed_string(std::string(components.getMemberNames().front()).c_str()));

  entt::meta_any any;
  if ( auto getFunc = type.func("Get"_hs) )
    any = getFunc.invoke(any, entt::forward_as_meta(registry), entity1);

  if ( auto deserialize = type.func("deserialize"_hs) )
    deserialize.invoke(any, entt::forward_as_meta(registry), entity1, entt::forward_as_meta(components["MyComponent"]));

  LOG_INFO("comp names after: {} {}", comp1.name, comp2.name);
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

//  testSerialization();
//  return;

  auto& fonts = mRegistry.ctx().at <FontAssetManager> ();
  auto& textures = mRegistry.ctx().at <TextureAssetManager> ();
  auto& strings = mRegistry.ctx().at <TextStringAssetManager> ();

  auto textureNull = texturePlaceholder();
  auto textureError = textureFromText("ERROR", olc::RED, true, mPGE);

  textures.insert("null", textureNull);
  textures.insert("error", textureError);

  auto& packageManager = mRegistry.ctx().at <PackageManager> ();

  try
  {
    packageManager.load("data/packages.json", mRegistry);
  }
  catch ( const std::exception& e )
  {
    LOG_ERROR("{}", e.what());
    return;
  }

  try
  {
    fonts.load({"munro"});
    textures.load({"map_diffuse"});
    textures.load({"map_height"});
    strings.load({"one_liner", "multi_liner"});
  }
  catch ( const std::exception& e )
  {
    LOG_ERROR("{}", e.what());
    return;
  }

  while (fonts.status("munro") != AssetStatus::Loaded)
    ;

  while (textures.status("map_height") != AssetStatus::Loaded)
    ;

  olc::Mouse::Cursor cursor {};
  cursor.bitmap = textures.get("map_height");
  cursor.hotspot = {cursor.bitmap->Sprite()->width / 2, cursor.bitmap->Sprite()->height / 2};

  mPGE->SetMouseCursor(cursor);

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

  ControlAxis iAxisCameraLookOn{};
  iAxisCameraLookOn.constraint = {0.0f, 1.0f};
  iAxisCameraLookOn.value = 0.0f;
  iAxisCameraLookOn.callbacks.insert(cqde::InputCallbackId("CameraLookOn"));

  ControlAxis iAxisCameraLookOff{};
  iAxisCameraLookOff.constraint = {0.0f, 1.0f};
  iAxisCameraLookOff.value = 0.0f;
  iAxisCameraLookOff.callbacks.insert(cqde::InputCallbackId("CameraLookOff"));

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
  cInputController.inputs[cqde::InputAxisId("CameraLookOn")] = iAxisCameraLookOn;
  cInputController.inputs[cqde::InputAxisId("CameraLookOff")] = iAxisCameraLookOff;
  cInputController.inputs[cqde::InputAxisId("CameraLookToggle")] = iAxisCameraLookToggle;
  cInputController.inputs[cqde::InputAxisId("EngineShutdown")] = iAxisExitListener;
  cInputController.inputs[cqde::InputAxisId("CursorPosX")] = iAxisCursorListener;

  auto& inputCallbacks = mRegistry.ctx().at <InputCallbackStorage> ();

  static float valPitch{};
  static float valYaw{};

  const auto cameraLookOn =
  [this, cursor] ( const entt::entity, InputController& cController )
  {
    mPGE->SetMouseCursor(olc::Mouse::Cursor{});
    mPGE->SetKeepMouseCentered(true);

    ControlAxis iAxisPitch{};
    iAxisPitch.constraint = {-90.0f, 90.0f};
    iAxisPitch.value = valPitch;

    ControlAxis iAxisYaw{};
    iAxisYaw.constraint = {-3600.0f, 3600.0f};
    iAxisYaw.value = valYaw;
    iAxisYaw.callbacks.insert(cqde::InputCallbackId("CameraYawClamp"));

    cController.inputs["Pitch"] = iAxisPitch;
    cController.inputs["Yaw"] = iAxisYaw;
  };

  const auto cameraLookOff =
  [this, cursor] ( const entt::entity, InputController& cController )
  {
    mPGE->SetMouseCursor(cursor);
    mPGE->SetKeepMouseCentered(false);

    valPitch = cController.inputs["Pitch"].value;
    valYaw = cController.inputs["Yaw"].value;

    cController.inputs.erase("Pitch");
    cController.inputs.erase("Yaw");
  };

  inputCallbacks.Register( cqde::InputCallbackId("CameraLookToggle"),
  [this, cursor, cameraLookOn, cameraLookOff] ( const entt::entity e, InputController& cController )
  {
    if ( cController.inputs.count("Pitch") > 0 || cController.inputs.count("Yaw") > 0 )
      cameraLookOff(e, cController);
    else
      cameraLookOn(e, cController);
  });

  inputCallbacks.Register( cqde::InputCallbackId("CameraLookOn"), cameraLookOn);
  inputCallbacks.Register( cqde::InputCallbackId("CameraLookOff"), cameraLookOff);

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
GameStateEcsSandbox::keyEvent( const olc::Event event )
{
  using namespace cqde::types;
  using cqde::InputHwCode;

  auto& inputManager = mRegistry.ctx().at <InputManager> ();

  const float inputDirection = event.type == olc::Event::KeyPressed
                          ? +1.0f
                          : -1.0f;

  inputManager.handleAxisInput( InputHwCode(event.key.code),
                                1.0f, inputDirection,
                                mRegistry );
}

void
GameStateEcsSandbox::mouseMoveEvent( const olc::Event::MouseMoveEvent event )
{
  using namespace cqde::types;
  using olc::MouseInputId;
  using cqde::InputHwCode;

  auto& inputManager = mRegistry.ctx().at <InputManager> ();

  if ( event.dx != 0 )
  {
    inputManager.handleAxisInput( InputHwCode(MouseInputId::MoveX),
                                  std::abs(event.dx), event.dx,
                                  mRegistry );
    inputManager.handleAxisInput( InputHwCode(MouseInputId::PosX),
                                  mPGE->GetMouseX(), event.dx,
                                  mRegistry );
  }

  if ( event.dy != 0 )
  {
    inputManager.handleAxisInput( InputHwCode(MouseInputId::MoveY),
                                  std::abs(event.dy), event.dy,
                                  mRegistry);
    inputManager.handleAxisInput( InputHwCode(MouseInputId::PosY),
                                  mPGE->GetMouseY(), event.dy,
                                  mRegistry);
  }
}

void
GameStateEcsSandbox::mouseButtonEvent( const olc::Event event )
{
  using namespace cqde::types;
  using olc::MouseInputId;
  using cqde::InputHwCode;

  auto& inputManager = mRegistry.ctx().at <InputManager> ();

  const InputHwCode inputHwCode = InputHwCode(MouseInputId::ENUM_BEGIN) + InputHwCode(event.mouseButton.button);

  const float inputDirection = event.type == olc::Event::MouseButtonPressed
                            ? 1.0f
                            : -1.0f;

  inputManager.handleAxisInput( inputHwCode,
                                1.0f, inputDirection,
                                mRegistry );
}

bool
GameStateEcsSandbox::update(  const uint32_t ticks,
                              const TimeUtils::Duration elapsed )
{
  using namespace cqde::compos;
  using cqde::InputAxisId;

  const double dt = static_cast <double> (elapsed);
  const float cameraVelocity = 10.0f;

// Camera control system
  for ( auto&& [eCamera, cCamera, cController, cTransform] : mRegistry.view <Camera, InputController, Transform>().each() )
  {
    const float translationX = cController.inputs[InputAxisId("TranslateX")].value * cameraVelocity * dt;
    cTransform.translation += cTransform.right() * translationX;

    const float translationY = cController.inputs[InputAxisId("TranslateY")].value * cameraVelocity * dt;
    cTransform.translation += cTransform.up() * translationY;

    const float translationZ = cController.inputs[InputAxisId("TranslateZ")].value * cameraVelocity * dt;
    cTransform.translation += cTransform.front() * translationZ;

    if ( cController.inputs.count("Pitch") == 0 || cController.inputs.count("Yaw") == 0 )
      continue;

    const float pitch = glm::radians( cController.inputs[InputAxisId("Pitch")].value );
    const float yaw = glm::radians( cController.inputs[InputAxisId("Yaw")].value );

    cTransform.orientation = glm::quat( {pitch, yaw, 0.0f} );
  }

  return mRunning;
}

void
GameStateEcsSandbox::render()
{
  cqde::systems::CullingSystem(mRegistry);
  cqde::systems::RenderSystem(mRegistry);

  auto& strings = mRegistry.ctx().at <cqde::types::TextStringAssetManager> ();
  mPGE->DrawStringDecal({0.0f, 0.0f}, *strings.try_get("multi_liner"_id));
}
