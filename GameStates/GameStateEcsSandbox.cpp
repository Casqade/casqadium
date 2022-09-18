#include <GameStateEcsSandbox.hpp>
#include <GameStateController.hpp>

#include <TimeUtils/Duration.hpp>

#include <cqde/alias.hpp>
#include <cqde/common.hpp>
#include <cqde/file_helpers.hpp>
#include <cqde/json_helpers.hpp>

#include <cqde/types/CallbackManager.hpp>
#include <cqde/types/EntityManager.hpp>
#include <cqde/types/PackageManager.hpp>
#include <cqde/types/SystemManager.hpp>

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

#include <glm/vec3.hpp>

#include <json/value.h>
#include <json/writer.h>


GameStateEcsSandbox::GameStateEcsSandbox(
  GameStateController* const stateController )
  : GameState(stateController)
{
  using namespace entt::literals;
  using namespace cqde::compos;
  using namespace cqde::types;

  cqde::engineInit(mRegistry);

//  entt::meta_ctx::bind(mRegistry.ctx().at <entt::meta_ctx> ());

  auto& fonts = mRegistry.ctx().at <FontAssetManager> ();
  auto& geometry = mRegistry.ctx().at <GeometryAssetManager> ();
  auto& textures = mRegistry.ctx().at <TextureAssetManager> ();

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

  fonts.load({"munro"});

  while (fonts.status("munro") != AssetStatus::Loaded)
    if ( fonts.status("munro") == AssetStatus::Error )
    {
      mRunning = false;
      return;
    }

  auto textRenderable = fonts.get("munro")->RenderStringToRenderable(U"T", olc::WHITE, false);
  auto textTexture = std::make_shared <olc::Renderable> (std::move(textRenderable));
  textures.insert("text_texture", textTexture);

  const auto engineShutdown =
  [this] (  entt::registry& registry,
            const std::vector <std::any>& args )
  {
    using cqde::fileOpen;
    using entt::type_hash;

    mRunning = false;

    const auto& entityManager = registry.ctx().at <EntityManager> ();

    const auto snapshot = entityManager.serialize(
      "editor",
      registry,
      {
        entityManager.componentType <Tag> (),
        entityManager.componentType <EntityMetaInfo> ()
      });

    const auto streamFlags = std::ios::out |
                             std::ios::trunc |
                             std::ios::binary;

    auto fileStream = fileOpen("snapshot.json", streamFlags);
    fileStream << Json::writeString(cqde::jsonWriter(), snapshot);
  };

  auto& callbackMgr = mRegistry.ctx().at <CallbackManager> ();
  callbackMgr.Register("EngineShutdown", engineShutdown);

  cqde::callbacks::editorModeEnable(mRegistry);
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
                                std::abs(event.delta), event.delta,
                                mRegistry );
}

bool
GameStateEcsSandbox::update(  const uint32_t ticks,
                              const TimeUtils::Duration& interval )
{
  using cqde::types::EntityManager;
  using cqde::types::SystemManager;
  using cqde::types::TickCurrent;
  using cqde::types::System;

  auto& entityManager = mRegistry.ctx().at <EntityManager> ();
  entityManager.delayedRemove(mRegistry);

  auto& tick = mRegistry.ctx().at <TickCurrent> ();

  tick.ticksElapsed = ticks;
  tick.tickInterval = interval;

  for ( uint32_t tick = 0; tick < ticks; ++tick )
    mRegistry.ctx().at <SystemManager> ().execute(mRegistry,
                                                  System::Phase::Logic);

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

  for ( uint32_t frame = 0; frame < frames; ++frame )
    mRegistry.ctx().at <SystemManager> ().execute(mRegistry,
                                                  System::Phase::Render);

  frame.lastFrameTimepoint = TimeUtils::Now();
}
