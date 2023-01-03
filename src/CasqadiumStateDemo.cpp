#include <CasqadiumStateDemo.hpp>

#include <TimeUtils/Duration.hpp>

#include <cqde/common.hpp>
#include <cqde/render_helpers.hpp>

#include <cqde/types/ConfigManager.hpp>
#include <cqde/types/CallbackManager.hpp>
#include <cqde/types/EntityManager.hpp>
#include <cqde/types/PackageManager.hpp>
#include <cqde/types/SystemManager.hpp>
#include <cqde/types/SnapshotManager.hpp>
#include <cqde/types/UserManager.hpp>

#include <cqde/types/TickCurrent.hpp>
#include <cqde/types/FrameCurrent.hpp>

#include <cqde/types/assets/AudioAssetManager.hpp>
#include <cqde/types/assets/FontAssetManager.hpp>
#include <cqde/types/assets/TextureAssetManager.hpp>

#include <cqde/types/input/InputManager.hpp>

#include <cqde/types/sequences/SequenceFactory.hpp>

#include <cqde/util/logger.hpp>

#include <cqde/components/Tag.hpp>
#include <cqde/components/EntityMetaInfo.hpp>

#include <cqde/callbacks/editor_input.hpp>

#include <demo/sequences.hpp>

#include <olcPGE/olcMouseInputId.hpp>
#include <olcPGE/olcPGEX_ImGui.hpp>

#include <soloud.h>


CasqadiumStateDemo::CasqadiumStateDemo(
  const ConfigManager& configManager )
{
  using namespace cqde::types;

  cqde::engineInit(mRegistry);

  entt::meta_ctx::bind(mRegistry.ctx().at <entt::meta_ctx> ());

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

  audioBackend.setMainResampler(configManager.audioResampler());
  audioBackend.setMaxActiveVoiceCount(configManager.audioMaxActiveVoices());

  const auto engineShutdown =
  [this] (  entt::registry& registry,
            const std::vector <std::any>& args )
  {
    mRunning = false;
  };

  auto& callbackMgr = mRegistry.ctx().at <CallbackManager> ();
  callbackMgr.Register("EngineShutdown", engineShutdown);

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

  if ( configManager.editorMode() == true )
    cqde::callbacks::editorModeEnable(mRegistry);
  else
    entityManager.entryPointExecute(mRegistry);
}

CasqadiumStateDemo::~CasqadiumStateDemo()
{
  mRegistry.clear();

  mRegistry.ctx().at <cqde::types::AudioAssetManager> ().clear(false);
}

void
CasqadiumStateDemo::keyEvent(
  const olc::Event event )
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
CasqadiumStateDemo::mouseMoveEvent(
  const olc::Event::MouseMoveEvent event )
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
CasqadiumStateDemo::mouseButtonEvent(
  const olc::Event event )
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
CasqadiumStateDemo::mouseWheelEvent(
  const olc::Event::MouseWheelScrollEvent event )
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
CasqadiumStateDemo::update(
  const uint32_t ticks,
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

  for ( uint32_t i = 0; i < ticks; ++i )
  {
    mRegistry.ctx().at <SystemManager> ().execute(mRegistry,
                                                  System::Phase::Logic);

    callbackManager.delayedExecution(mRegistry);
    entityManager.delayedRemove(mRegistry);

    tick.lastTickTimepoint = TimeUtils::Now();
  }

  return mRunning;
}

void
CasqadiumStateDemo::render(
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
