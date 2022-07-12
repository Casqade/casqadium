#include <GameStateEcsSandbox.hpp>
#include <GameStateController.hpp>

#include <TimeUtils/Duration.hpp>

#include <entt/entt.hpp>
#include <entt/core/hashed_string.hpp>

#include <cqde/alias.hpp>
#include <cqde/common.hpp>
#include <cqde/ecs_helpers.hpp>

#include <cqde/types/CallbackManager.hpp>
#include <cqde/types/EntityManager.hpp>
#include <cqde/types/PackageManager.hpp>

#include <cqde/types/assets/FontAssetManager.hpp>
#include <cqde/types/assets/GeometryAssetManager.hpp>
#include <cqde/types/assets/TextureAssetManager.hpp>
#include <cqde/types/assets/TextStringAssetManager.hpp>

#include <cqde/types/input/InputManager.hpp>
#include <cqde/types/input/InputBinding.hpp>
#include <cqde/types/input/InputBindingAbsolute.hpp>
#include <cqde/types/input/InputBindingRelative.hpp>

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

#include <glm/vec3.hpp>

#include <json/value.h>


static std::shared_ptr <olc::Renderable>
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

static std::shared_ptr <olc::Renderable>
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

static std::shared_ptr <std::vector <glm::vec3>>
geometryPlaceholder()
{
  return std::make_shared <std::vector <glm::vec3>> (
  std::vector <glm::vec3>
  {
    {-0.5f, 0.5f, 0.0f},
    {-0.5f, -0.5f, 0.0f},
    {0.5f, -0.5f, 0.0f},
    {0.5f, 0.5f, 0.0f}
  });
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
  using cqde::InputCallbackId;

  cqde::engineInit(mRegistry);

//  entt::meta_ctx::bind(mRegistry.ctx().at <entt::meta_ctx> ());

//  testSerialization();
//  return;

  auto& fonts = mRegistry.ctx().at <FontAssetManager> ();
  auto& geometry = mRegistry.ctx().at <GeometryAssetManager> ();
  auto& textures = mRegistry.ctx().at <TextureAssetManager> ();
  auto& strings = mRegistry.ctx().at <TextStringAssetManager> ();

  auto textureError = textureFromText("ERROR", olc::RED, true, mPGE);

  geometry.insert("null", geometryPlaceholder());
  geometry.insert("error", geometryPlaceholder());

  textures.insert("null", texturePlaceholder());
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
    geometry.load({"quad"});
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

  auto& callbackMgr = mRegistry.ctx().at <CallbackManager> ();

  static float valPitch{};
  static float valYaw{};

  const auto cameraLookOn =
  [this, cursor] (  entt::registry& registry,
                    const std::vector <std::any>& args )
  {
    auto cController = std::any_cast <InputController*> (args.at(1));

    mPGE->SetMouseCursor(olc::Mouse::Cursor{});
    mPGE->SetKeepMouseCentered(true);

    ControlAxis iAxisPitch{};
    iAxisPitch.constraint = {-90.0f, 90.0f};
    iAxisPitch.value = valPitch;

    ControlAxis iAxisYaw{};
    iAxisYaw.constraint = {-3600.0f, 3600.0f};
    iAxisYaw.value = valYaw;
    iAxisYaw.callbacks.insert("CameraYawClamp"_id);

    cController->inputs["Pitch"] = iAxisPitch;
    cController->inputs["Yaw"] = iAxisYaw;
  };

  const auto cameraLookOff =
  [this, cursor] (  entt::registry& registry,
                    const std::vector <std::any>& args )
  {
    auto cController = std::any_cast <InputController*> (args.at(1));

    mPGE->SetMouseCursor(cursor);
    mPGE->SetKeepMouseCentered(false);

    valPitch = cController->inputs["Pitch"].value;
    valYaw = cController->inputs["Yaw"].value;

    cController->inputs.erase("Pitch");
    cController->inputs.erase("Yaw");
  };

  const auto cameraLookToggle =
  [cursor, cameraLookOn, cameraLookOff] ( entt::registry& registry,
                                          const std::vector <std::any>& args )
  {
    auto entity = std::any_cast <entt::entity> (args.at(0));
    auto cController = std::any_cast <InputController*> (args.at(1));

    if ( cController->inputs.count("Pitch") > 0 || cController->inputs.count("Yaw") > 0 )
      cameraLookOff( registry, {entity, cController} );
    else
      cameraLookOn( registry, {entity, cController} );
  };

  const auto cameraYawClamp =
  [] (  entt::registry& registry,
        const std::vector <std::any>& args )
  {
    auto cController = std::any_cast <InputController*> (args.at(1));

    if ( cController->inputs.count("Yaw") == 0 )
      return;

    float& yaw = cController->inputs["Yaw"].value;
    yaw = yaw > 180.0f ? yaw - 360.0f : yaw;
    yaw = yaw < -180.0f ? yaw + 360.0f : yaw;
  };

  const auto quitGame =
  [this] (  entt::registry& registry,
            const std::vector <std::any>& args )
  {
    mRunning = false;

    auto& entityManager = registry.ctx().at <EntityManager> ();
    auto& inputManager = registry.ctx().at <InputManager> ();

    entityManager.save("entities.json", "editor"_id,
                       registry,
                       {
                         entt::type_hash <Tag>::value(),
                         entt::type_hash <EntityMetaInfo>::value()
                       });

    inputManager.save("input.json");
  };

  callbackMgr.Register("CameraLookOn"_id, cameraLookOn);
  callbackMgr.Register("CameraLookOff"_id, cameraLookOff);
  callbackMgr.Register("CameraLookToggle"_id, cameraLookToggle);
  callbackMgr.Register("CameraYawClamp"_id, cameraYawClamp);
  callbackMgr.Register("QuitGame"_id, quitGame);
}

void
GameStateEcsSandbox::keyEvent( const olc::Event event )
{
  using cqde::InputHwCode;
  using cqde::types::InputManager;

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
  using olc::MouseInputId;
  using cqde::InputHwCode;
  using cqde::types::InputManager;

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
  using olc::MouseInputId;
  using cqde::InputHwCode;
  using cqde::types::InputManager;

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
                              const TimeUtils::Duration& interval )
{
  using namespace cqde::compos;
  using cqde::InputAxisId;

  const double dt = static_cast <double> (elapsed);
  const float cameraVelocity = 10.0f;

// Camera control system
  for ( auto&& [eCamera,
                cCamera,
                cController,
                cTransform] : mRegistry.view <Camera,
                                              InputController,
                                              Transform>().each() )
  {
    const float translationX = cController.inputs["TranslateX"_id].value * cameraVelocity * dt;
    cTransform.translation += cTransform.right() * translationX;

    const float translationY = cController.inputs["TranslateY"_id].value * cameraVelocity * dt;
    cTransform.translation += cTransform.up() * translationY;

    const float translationZ = cController.inputs["TranslateZ"_id].value * cameraVelocity * dt;
    cTransform.translation += cTransform.front() * translationZ;

    if ( cController.inputs.count("Pitch") == 0 || cController.inputs.count("Yaw") == 0 )
      continue;

    const float pitch = glm::radians( cController.inputs["Pitch"_id].value );
    const float yaw = glm::radians( cController.inputs["Yaw"_id].value );

    cTransform.orientation = glm::quat( {pitch, yaw, 0.0f} );
  }

  return mRunning;
}

void
GameStateEcsSandbox::render(
  const uint32_t frames,
  const TimeUtils::Duration& interval )
{
  using fmt::format;
  using cqde::systems::CullingSystem;
  using cqde::systems::RenderSystem;
  using cqde::types::InputManager;
  using cqde::types::TextStringAssetManager;

  CullingSystem(mRegistry);
  RenderSystem(mRegistry);
}
