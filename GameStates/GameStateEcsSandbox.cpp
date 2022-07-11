#include <GameStateEcsSandbox.hpp>
#include <GameStateController.hpp>

#include <TimeUtils/Duration.hpp>

#include <entt/entt.hpp>
#include <entt/core/hashed_string.hpp>

#include <cqde/alias.hpp>
#include <cqde/common.hpp>
#include <cqde/ecs_helpers.hpp>

#include <cqde/types/EntityManager.hpp>
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
#include <cqde/components/EntityMetaInfo.hpp>
#include <cqde/components/GeometryBuffer.hpp>
#include <cqde/components/InputController.hpp>
#include <cqde/components/TextureBuffer.hpp>

#include <cqde/systems/RenderSystem.hpp>
#include <cqde/systems/CullingSystem.hpp>

#include <olcPGE/olcMouseInputId.hpp>

#include <spdlog/fmt/bundled/format.h>

#include <json/value.h>


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

//  entt::meta_ctx::bind(mRegistry.ctx().at <entt::meta_ctx> ());

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

  const auto cameraLookToggle =
  [this, cursor, cameraLookOn, cameraLookOff] ( const entt::entity e, InputController& cController )
  {
    if ( cController.inputs.count("Pitch") > 0 || cController.inputs.count("Yaw") > 0 )
      cameraLookOff(e, cController);
    else
      cameraLookOn(e, cController);
  };

  const auto cameraYawClamp =
  [] ( const entt::entity, InputController& cController )
  {
    if ( cController.inputs.count("Yaw") == 0 )
      return;

    float& yaw = cController.inputs["Yaw"].value;
    yaw = yaw > 180.0f ? yaw - 360.0f : yaw;
    yaw = yaw < -180.0f ? yaw + 360.0f : yaw;
  };

  const auto quitGame =
  [this] ( const entt::entity, InputController& )
  {
    mRunning = false;

    auto& entityManager = mRegistry.ctx().at <EntityManager> ();
    auto& inputManager = mRegistry.ctx().at <InputManager> ();

    entityManager.save("entities.json", "editor"_id,
                       mRegistry,
                       {
                         entt::type_hash <Tag>::value(),
                         entt::type_hash <EntityMetaInfo>::value()
                       });

    inputManager.save("input.json");
  };

  inputCallbacks.Register(cqde::InputCallbackId("CameraLookOn"), cameraLookOn);
  inputCallbacks.Register(cqde::InputCallbackId("CameraLookOff"), cameraLookOff);
  inputCallbacks.Register(cqde::InputCallbackId("CameraLookToggle"), cameraLookToggle);
  inputCallbacks.Register(cqde::InputCallbackId("CameraYawClamp"), cameraYawClamp);
  inputCallbacks.Register(cqde::InputCallbackId("QuitGame"), quitGame);
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
  using fmt::format;

  cqde::systems::CullingSystem(mRegistry);
  cqde::systems::RenderSystem(mRegistry);

  auto now = TimeUtils::Now();

  auto& inputMgr = mRegistry.ctx().at <cqde::types::InputManager> ();

  uint8_t eventsToDisplay = 40;
  float textY = 0.0f;
  float textH = 8.0f;

  for ( auto iter = inputMgr.inputHistory().rbegin();
        iter != inputMgr.inputHistory().rend();
        ++iter )
  {
    if ( eventsToDisplay == 0 )
      break;

    --eventsToDisplay;

    auto& event = *iter;
    mPGE->DrawStringDecal({0.0f, textY += textH},
                          format("{}: {} -{:.2f}",
                                  event.inputId.str(), event.amount, double(now - event.tp)));
  }

  auto& strings = mRegistry.ctx().at <cqde::types::TextStringAssetManager> ();
  mPGE->DrawStringDecal({0.0f, textY += textH}, *strings.try_get("multi_liner"_id));
}
