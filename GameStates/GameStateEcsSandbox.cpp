#include <GameStateEcsSandbox.hpp>
#include <GameStateController.hpp>

#include <TimeUtils/Duration.hpp>

#include <cqde/alias.hpp>
#include <cqde/common.hpp>
#include <cqde/file_helpers.hpp>
#include <cqde/json_helpers.hpp>

#include <cqde/types/ConfigManager.hpp>
#include <cqde/types/CallbackManager.hpp>
#include <cqde/types/EntityManager.hpp>
#include <cqde/types/PackageManager.hpp>
#include <cqde/types/SystemManager.hpp>
#include <cqde/types/SnapshotManager.hpp>
#include <cqde/types/UserManager.hpp>

#include <cqde/types/TickCurrent.hpp>
#include <cqde/types/FrameCurrent.hpp>

#include <cqde/types/assets/FontAssetManager.hpp>
#include <cqde/types/assets/GeometryAssetManager.hpp>
#include <cqde/types/assets/TextureAssetManager.hpp>
#include <cqde/types/assets/TextStringAssetManager.hpp>

#include <cqde/types/input/InputManager.hpp>

#include <cqde/util/logger.hpp>

#include <cqde/components/Tag.hpp>
#include <cqde/components/EntityMetaInfo.hpp>

#include <cqde/callbacks.hpp>
#include <cqde/systems.hpp>

#include <olcPGE/olcMouseInputId.hpp>
#include <olcPGE/olcPGEX_ImGui.hpp>

#include <soloud.h>

#include <glm/vec3.hpp>

#include <json/value.h>
#include <json/writer.h>


GameStateEcsSandbox::GameStateEcsSandbox(
  GameStateController* const stateController,
  const ConfigManager& configManager )
  : GameState(stateController)
{
  using namespace cqde::types;

  cqde::engineInit(mRegistry);

//  entt::meta_ctx::bind(mRegistry.ctx().at <entt::meta_ctx> ());

  mRegistry.ctx().emplace <ConfigManager> (configManager);

  auto& audioBackend = mRegistry.ctx().at <SoLoud::Soloud> ();

  const auto audioInitResult = audioBackend.init(
    configManager.audioFlags(),
    configManager.audioBackend(),
    configManager.audioSampleRate(),
    configManager.audioBufferSize() );

  if ( audioInitResult != SoLoud::SOLOUD_ERRORS::SO_NO_ERROR )
  {
    mRunning = false;
    LOG_ERROR("Failed to initialize SoLoud: {}",
              audioBackend.getErrorString(audioInitResult));
    return;
  }

  auto& packageManager = mRegistry.ctx().at <PackageManager> ();

  try
  {
    packageManager.load(mRegistry);
  }
  catch ( const std::exception& e )
  {
    LOG_ERROR("{}", e.what());

    mRunning = false;
    return;
  }

  auto& userManager = mRegistry.ctx().at <UserManager> ();
  userManager.setUser(configManager.lastUser());

  auto& inputManager = mRegistry.ctx().at <InputManager> ();
  inputManager.load(userManager.inputConfigPath());

  auto& entityManager = mRegistry.ctx().at <EntityManager> ();
  entityManager.entryPointExecute(mRegistry);

  const cqde::identifier fontId = "munro";

  auto& fonts = mRegistry.ctx().at <FontAssetManager> ();
  fonts.load({fontId});

  while ( fonts.status(fontId) != AssetStatus::Loaded )
    if ( fonts.status(fontId) == AssetStatus::Error )
    {
      mRunning = false;
      return;
    }

  auto textRenderable = fonts.get(fontId)->RenderStringToRenderable(U"T", olc::WHITE, false);
  auto textTexture = std::make_shared <olc::Renderable> (std::move(textRenderable));

  auto& textures = mRegistry.ctx().at <TextureAssetManager> ();
  textures.insert("text_texture", textTexture);

  const auto engineShutdown =
  [this] (  entt::registry& registry,
            const std::vector <std::any>& args )
  {
    registry.clear();
    mRunning = false;
  };

  auto& callbackMgr = mRegistry.ctx().at <CallbackManager> ();
  callbackMgr.Register("EngineShutdown", engineShutdown);

  cqde::callbacks::editorModeEnable(mRegistry);
}

GameStateEcsSandbox::~GameStateEcsSandbox()
{
  mRegistry.clear();
}

void
GameStateEcsSandbox::keyEvent( const olc::Event event )
{
  if ( ImGui::GetIO().WantCaptureKeyboard == true ||
       ImGui::GetIO().WantTextInput == true )
  {
    LOG_INFO("+key {}", event.key.code);
    return;
  }

  using cqde::InputHwCode;
  using cqde::types::InputManager;

  auto& inputManager = mRegistry.ctx().at <InputManager> ();

  const int8_t inputDirection = event.type - olc::Event::KeyHeld;

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
                                  std::abs(event.dx),
                                  std::clamp(event.dx, -1, 1),
                                  mRegistry );
    inputManager.handleAxisInput( InputHwCode(MouseInputId::PosX),
                                  mPGE->GetMouseX(),
                                  std::clamp(event.dx, -1, 1),
                                  mRegistry );
  }

  if ( event.dy != 0 )
  {
    inputManager.handleAxisInput( InputHwCode(MouseInputId::MoveY),
                                  std::abs(event.dy),
                                  std::clamp(event.dy, -1, 1),
                                  mRegistry);
    inputManager.handleAxisInput( InputHwCode(MouseInputId::PosY),
                                  mPGE->GetMouseY(),
                                  std::clamp(event.dy, -1, 1),
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

  const InputHwCode inputHwCode = InputHwCode(MouseInputId::ENUM_BEGIN) +
                                  InputHwCode(event.mouseButton.button);

  const int8_t inputDirection = event.type - olc::Event::MouseButtonHeld;

  inputManager.handleAxisInput( inputHwCode,
                                1.0f, inputDirection,
                                mRegistry );
}

void
GameStateEcsSandbox::mouseWheelEvent( const olc::Event::MouseWheelScrollEvent event )
{
  using olc::MouseInputId;
  using cqde::InputHwCode;
  using cqde::types::InputManager;

  auto& inputManager = mRegistry.ctx().at <InputManager> ();

  inputManager.handleAxisInput( InputHwCode(MouseInputId::Wheel),
                                std::abs(event.delta),
                                std::clamp(event.delta, -1, 1),
                                mRegistry );
}

bool
GameStateEcsSandbox::update(  const uint32_t ticks,
                              const TimeUtils::Duration& interval )
{
  using cqde::types::CallbackManager;
  using cqde::types::EntityManager;
  using cqde::types::SystemManager;
  using cqde::types::TickCurrent;
  using cqde::types::System;

  auto& callbackManager = mRegistry.ctx().at <CallbackManager> ();
  auto& entityManager = mRegistry.ctx().at <EntityManager> ();

  auto& tick = mRegistry.ctx().at <TickCurrent> ();

  tick.ticksElapsed = ticks;
  tick.tickInterval = interval;

  for ( uint32_t tick = 0; tick < ticks; ++tick )
  {
    mRegistry.ctx().at <SystemManager> ().execute(mRegistry,
                                                  System::Phase::Logic);

    callbackManager.delayedExecution(mRegistry);
    entityManager.delayedRemove(mRegistry);
  }

  tick.lastTickTimepoint = TimeUtils::Now();

  return mRunning;
}

void
GameStateEcsSandbox::render(
  const uint32_t frames,
  const TimeUtils::Duration& interval )
{
  using cqde::types::SystemManager;
  using cqde::types::FrameCurrent;
  using cqde::types::System;

  auto& frame = mRegistry.ctx().at <FrameCurrent> ();

  frame.framesElapsed = frames;
  frame.frameInterval = interval;

  mRegistry.ctx().at <SystemManager> ().execute(mRegistry,
                                                System::Phase::Render);

  frame.lastFrameTimepoint = TimeUtils::Now();
}
