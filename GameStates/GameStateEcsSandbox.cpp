#include <GameStateEcsSandbox.hpp>
#include <GameStateController.hpp>

#include <TimeUtils/Duration.hpp>

#include <entt/entt.hpp>
#include <entt/core/hashed_string.hpp>

#include <cqde/alias.hpp>
#include <cqde/common.hpp>
#include <cqde/ecs_helpers.hpp>
#include <cqde/math_helpers.hpp>
#include <cqde/render_helpers.hpp>

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
#include <cqde/types/input/InputBinding.hpp>
#include <cqde/types/input/InputBindingAbsolute.hpp>
#include <cqde/types/input/InputBindingRelative.hpp>

#include <cqde/types/ui/AssetManagerUi.hpp>
#include <cqde/types/ui/CallbackManagerUi.hpp>
#include <cqde/types/ui/EntityManagerUi.hpp>
#include <cqde/types/ui/InputManagerUi.hpp>
#include <cqde/types/ui/PackageManagerUi.hpp>
#include <cqde/types/ui/SystemManagerUi.hpp>
#include <cqde/types/ui/ViewportManagerUi.hpp>

#include <cqde/util/logger.hpp>

#include <cqde/components/Tag.hpp>
#include <cqde/components/Camera.hpp>
#include <cqde/components/SceneNode.hpp>
#include <cqde/components/Transform.hpp>
#include <cqde/components/EntityMetaInfo.hpp>
#include <cqde/components/GeometryBuffer.hpp>
#include <cqde/components/InputController.hpp>
#include <cqde/components/SubscriberInput.hpp>
#include <cqde/components/TextureBuffer.hpp>
#include <cqde/components/SubscriberUpdate.hpp>

#include <cqde/systems/RenderSystem.hpp>
#include <cqde/systems/CullingSystem.hpp>

#include <olcPGE/olcMouseInputId.hpp>
#include <olcPGE/olcPGEX_ImGui.hpp>

#include <glm/vec3.hpp>

#include <json/value.h>


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
    packageManager.load("data/packages.json", mRegistry);
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

  const auto mouseAutoCenterEnable =
  [this] (  entt::registry& registry,
            const std::vector <std::any>& args )
  {
    mPGE->SetKeepMouseCentered(true);
  };

  const auto mouseAutoCenterDisable =
  [this] (  entt::registry& registry,
            const std::vector <std::any>& args )
  {
    mPGE->SetKeepMouseCentered(false);
  };

  const auto mouseCursorHide =
  [this] (  entt::registry& registry,
            const std::vector <std::any>& args )
  {
    mPGE->SetMouseCursor(olc::Mouse::Cursor{});
  };

  const auto mouseCursorShow =
  [this] (  entt::registry& registry,
            const std::vector <std::any>& args )
  {
    mPGE->ResetMouseCursor();
  };

  const auto entityInputOn =
  [] (  entt::registry& registry,
        const std::vector <std::any>& args )
  {
    const auto entity = std::any_cast <entt::entity> (args.at(0));
    registry.emplace_or_replace <SubscriberInput> (entity);
  };

  const auto entityInputOff =
  [] (  entt::registry& registry,
        const std::vector <std::any>& args )
  {
    const auto entity = std::any_cast <entt::entity> (args.at(0));
    registry.remove <SubscriberInput> (entity);
  };

  const auto entityActivate =
  [] (  entt::registry& registry,
        const std::vector <std::any>& args )
  {
    const auto entity = std::any_cast <entt::entity> (args.at(0));
    registry.emplace_or_replace <SubscriberUpdate> (entity);
  };

  const auto entityDeactivate =
  [] (  entt::registry& registry,
        const std::vector <std::any>& args )
  {
    const auto entity = std::any_cast <entt::entity> (args.at(0));
    registry.remove <SubscriberUpdate> (entity);
  };

  const auto editorCameraControlOn =
  [entityInputOn, mouseCursorHide, mouseAutoCenterEnable] (
    entt::registry& registry,
    const std::vector <std::any>& args )
  {
    using cqde::ui::ViewportManagerUi;

    if ( registry.ctx().at <ViewportManagerUi> ().mouseOverViewport("cqde_editor_camera") == false )
      return;

    auto& entityManager = registry.ctx().at <EntityManager> ();

    const auto eCamera = entityManager.get_if_valid("cqde_editor_camera", registry);

    if ( eCamera == entt::null )
      return;

    if ( registry.all_of <SubscriberInput> (eCamera) == true )
      return;

    entityInputOn(registry, {eCamera});

    mouseAutoCenterEnable(registry, args);
    mouseCursorHide(registry, args);
  };

  const auto controlTranslateXRelative =
  [] (  entt::registry& registry,
        const std::vector <std::any>& args )
  {
    const auto entity = std::any_cast <entt::entity> (args.at(0));
    auto cController = std::any_cast <InputController*> (args.at(1));

    const auto& tick = registry.ctx().at <TickCurrent> ();
    const auto ticks = tick.ticksElapsed;
    const auto elapsed = tick.tickInterval;

    const float dt = ticks * static_cast <double> (elapsed);

    auto& cTransform = registry.get <Transform> (entity);

    auto& axisValue = cController->inputs["TranslateX"].value;

    cTransform.translation += cTransform.right() * axisValue * dt;

    axisValue = 0.0f;
  };

  const auto controlTranslateYRelative =
  [] (  entt::registry& registry,
        const std::vector <std::any>& args )
  {
    const auto entity = std::any_cast <entt::entity> (args.at(0));
    auto cController = std::any_cast <InputController*> (args.at(1));

    const auto& tick = registry.ctx().at <TickCurrent> ();
    const auto ticks = tick.ticksElapsed;
    const auto elapsed = tick.tickInterval;

    const float dt = ticks * static_cast <double> (elapsed);

    auto& cTransform = registry.get <Transform> (entity);

    auto& axisValue = cController->inputs["TranslateY"].value;

    cTransform.translation += cTransform.up() * axisValue * dt;

    axisValue = 0.0f;
  };

  const auto controlTranslateZRelative =
  [] (  entt::registry& registry,
        const std::vector <std::any>& args )
  {
    const auto entity = std::any_cast <entt::entity> (args.at(0));
    auto cController = std::any_cast <InputController*> (args.at(1));

    const auto& tick = registry.ctx().at <TickCurrent> ();
    const auto ticks = tick.ticksElapsed;
    const auto elapsed = tick.tickInterval;

    const float dt = ticks * static_cast <double> (elapsed);

    auto& cTransform = registry.get <Transform> (entity);

    auto& axisValue = cController->inputs["TranslateZ"].value;

    cTransform.translation += cTransform.front() * axisValue * dt;

    axisValue = 0.0f;
  };

  const auto controlPitchRelative =
  [] (  entt::registry& registry,
        const std::vector <std::any>& args )
  {
    const auto entity = std::any_cast <entt::entity> (args.at(0));
    auto cController = std::any_cast <InputController*> (args.at(1));

    auto& cTransform = registry.get <Transform> (entity);

    auto& axisValue = cController->inputs["Pitch"].value;

    const auto angle = glm::angleAxis(glm::radians(axisValue),
                                      glm::vec3 {1.0f, 0.0f, 0.0f});

    const auto orientationPrev = cTransform.orientation;

    cTransform.orientation = glm::normalize(cTransform.orientation * angle);

    if ( cTransform.up().y < 0.0f )
      cTransform.orientation = orientationPrev;

    axisValue = 0.0f;
  };

  const auto controlYawRelative =
  [] (  entt::registry& registry,
        const std::vector <std::any>& args )
  {
    const auto entity = std::any_cast <entt::entity> (args.at(0));
    auto cController = std::any_cast <InputController*> (args.at(1));

    auto& cTransform = registry.get <Transform> (entity);

    auto& axisValue = cController->inputs["Yaw"].value;

    const auto angle = glm::angleAxis(glm::radians(axisValue),
                                      glm::vec3 {0.0f, 1.0f, 0.0f});

    cTransform.orientation = glm::normalize(angle * cTransform.orientation);

    axisValue = 0.0f;
  };

  const auto controlRollRelative =
  [] (  entt::registry& registry,
        const std::vector <std::any>& args )
  {
    const auto entity = std::any_cast <entt::entity> (args.at(0));
    auto cController = std::any_cast <InputController*> (args.at(1));

    auto& cTransform = registry.get <Transform> (entity);

    auto& axisValue = cController->inputs["Roll"].value;

    const auto angle = glm::angleAxis(glm::radians(axisValue),
                                      glm::vec3 {0.0f, 0.0f, 1.0f});

    cTransform.orientation = glm::normalize(angle * cTransform.orientation);

    axisValue = 0.0f;
  };

  const auto engineShutdown =
  [this] (  entt::registry& registry,
            const std::vector <std::any>& args )
  {
    using entt::type_hash;

    mRunning = false;

    registry.ctx().at <EntityManager> ().save(
      "entities.json", "editor",
      registry,
      {
        type_hash <Tag> (),
        type_hash <EntityMetaInfo> ()
      });
  };

  const auto editorCameraFovControl =
  [] (  entt::registry& registry,
        const std::vector <std::any>& args )
  {
    const auto eCamera = std::any_cast <entt::entity> (args.at(0));
    const auto cController = std::any_cast <InputController*> (args.at(1));

    auto& cCamera = registry.get <Camera> (eCamera);

    if ( cCamera.projectionType == Camera::Projection::Perspective )
      cCamera.fov = cController->inputs["EditorCameraFov"].value;
  };

  const auto editorCameraZoomControl =
  [] (  entt::registry& registry,
        const std::vector <std::any>& args )
  {
    const auto eCamera = std::any_cast <entt::entity> (args.at(0));
    const auto cController = std::any_cast <InputController*> (args.at(1));

    auto& cCamera = registry.get <Camera> (eCamera);

    if ( cCamera.projectionType == Camera::Projection::Orthographic )
      cCamera.fov = cController->inputs["EditorCameraZoom"].value;
  };

  const auto editorEntitySelect =
  [] (  entt::registry& registry,
        const std::vector <std::any>& args )
  {
    using cqde::drawLines;
    using cqde::boundingBox;
    using cqde::pointInRect;
    using cqde::LineRenderMode;

    using cqde::ui::ViewportManagerUi;
    using cqde::ui::EntityManagerUi;

    const auto cController = std::any_cast <InputController*> (args.at(1));

    const auto& viewportManagerUi = registry.ctx().at <ViewportManagerUi> ();

    if ( viewportManagerUi.mouseOverViewport("cqde_editor_camera") == false )
      return;

    const glm::vec2 cursorPos
    {
      cController->inputs["CursorPosX"].value,
      cController->inputs["CursorPosY"].value,
    };

    const auto& entityManager = registry.ctx().at <EntityManager> ();
    auto& entityManagerUi = registry.ctx().at <EntityManagerUi> ();

    const auto eCamera = entityManager.get("cqde_editor_camera");

    if ( registry.all_of <SubscriberInput> (eCamera) == true )
      return;

    auto& cCamera = registry.get <Camera> (eCamera);

    for ( auto iter = cCamera.zBuffer.rbegin();
          iter != cCamera.zBuffer.rend();
          ++iter )
    {
      const auto& [vBuf, entity] = *iter;

      if ( pointInRect( cursorPos, boundingBox(vBuf.vertices) ) == false )
        continue;

      if ( entityManagerUi.selectedEntity() == entity )
        continue;

      return entityManagerUi.entitySelect(entity);
    }

    entityManagerUi.entitySelect(entt::null);
  };

  const auto EditorEntityHighlightSystem =
  [] ( entt::registry& registry )
  {
    using cqde::drawLines;
    using cqde::LineRenderMode;
    using cqde::ui::EntityManagerUi;

    const auto& entityManager   = registry.ctx().at <EntityManager> ();
    const auto& entityManagerUi = registry.ctx().at <EntityManagerUi> ();

    const auto selectedEntity = entityManagerUi.selectedEntity();

    if ( selectedEntity == entt::null )
      return;

    const auto eCamera = entityManager.get("cqde_editor_camera");
    if ( eCamera == entt::null )
      return;

    const auto& cCamera = registry.get <Camera> (eCamera);

    for ( const auto& [vBuf, entity] : cCamera.zBuffer )
      if ( entity == selectedEntity )
        drawLines(vBuf.vertices, olc::YELLOW, LineRenderMode::Loop);
  };

  const auto EditorSystem =
  [this] ( entt::registry& registry )
  {
    using cqde::ui::AssetManagerUi;
    using cqde::ui::CallbackManagerUi;
    using cqde::ui::EntityManagerUi;
    using cqde::ui::InputManagerUi;
    using cqde::ui::PackageManagerUi;
    using cqde::ui::SystemManagerUi;
    using cqde::ui::ViewportManagerUi;

    auto& entityManager = registry.ctx().at <EntityManager> ();
    entityManager.delayedRemove(registry);

    ImGui::PushStyleColor(ImGuiCol_WindowBg, 0);
    ImGui::DockSpaceOverViewport(NULL, ImGuiDockNodeFlags_PassthruCentralNode);
    ImGui::PopStyleColor();

    registry.ctx().at <PackageManagerUi> ().ui_show(registry);
    registry.ctx().at <AssetManagerUi> ().ui_show(registry);
    registry.ctx().at <CallbackManagerUi> ().ui_show(registry);
    registry.ctx().at <EntityManagerUi> ().ui_show(registry);
    registry.ctx().at <InputManagerUi> ().ui_show(registry);
    registry.ctx().at <SystemManagerUi> ().ui_show(registry);
    registry.ctx().at <ViewportManagerUi> ().ui_show(registry);

    if ( entityManager.get_if_valid("cqde_editor_camera", registry) == entt::null )
    {
      auto eCamera = registry.create();
      auto& cTag = registry.emplace <Tag> (eCamera);

      cTag.id = "cqde_editor_camera";
      entityManager.idRegister(cTag.id, eCamera);

      registry.emplace <SubscriberUpdate> (eCamera);

      auto& cMetaInfo = registry.emplace <EntityMetaInfo> (eCamera);
      cMetaInfo.packageId = "";

      auto& cCamera = registry.emplace <Camera> (eCamera);
      auto& cTransform = registry.emplace <Transform> (eCamera);
      auto& cSceneNode = registry.emplace <SceneNode> (eCamera);

      cCamera.renderMode = Camera::RenderMode::Wireframe;

      auto& cInputController = registry.emplace <InputController> (eCamera);

      auto& iTranslateX = cInputController.inputs["TranslateX"];
      auto& iTranslateY = cInputController.inputs["TranslateY"];
      auto& iTranslateZ = cInputController.inputs["TranslateZ"];

      iTranslateX.constraint = {-1.0f, 1.0f};
      iTranslateY.constraint = {-1.0f, 1.0f};
      iTranslateZ.constraint = {-1.0f, 1.0f};

      iTranslateX.callbacks.insert("ControlTranslateXRelative");
      iTranslateY.callbacks.insert("ControlTranslateYRelative");
      iTranslateZ.callbacks.insert("ControlTranslateZRelative");

      auto& iPitch = cInputController.inputs["Pitch"];
      auto& iYaw = cInputController.inputs["Yaw"];
      auto& iRoll = cInputController.inputs["Roll"];

      iPitch.constraint = {1.0f, 0.0f};
      iYaw.constraint = {1.0f, 0.0f};
      iRoll.constraint = {1.0f, 0.0f};

      iPitch.callbacks.insert("ControlPitchRelative");
      iYaw.callbacks.insert("ControlYawRelative");
      iRoll.callbacks.insert("ControlRollRelative");

      auto& iCameraControlOff = cInputController.inputs["EditorCameraControlOff"];

      iCameraControlOff.callbacks.insert("EntityInputOff");
      iCameraControlOff.callbacks.insert("MouseAutoCenterDisable");
      iCameraControlOff.callbacks.insert("MouseCursorShow");

      auto& iCameraFov = cInputController.inputs["EditorCameraFov"];
      iCameraFov.value = cCamera.fov;
      iCameraFov.callbacks.insert("EditorCameraFovControl");
      iCameraFov.constraint = { glm::epsilon <float> (),
                                glm::pi <float> () - glm::epsilon <float> () };

      auto& iCameraZoom = cInputController.inputs["EditorCameraZoom"];
      iCameraZoom.value = 0.01;
      iCameraZoom.callbacks.insert("EditorCameraZoomControl");
      iCameraZoom.constraint = {  std::numeric_limits <float>::min(),
                                  std::numeric_limits <float>::max() };
    }

    if ( entityManager.get_if_valid("cqde_editor_controller", registry) == entt::null )
    {
      auto eEditorController = registry.create();
      auto& cTag = registry.emplace <Tag> (eEditorController);

      cTag.id = "cqde_editor_controller";
      entityManager.idRegister(cTag.id, eEditorController);

      auto& cMetaInfo = registry.emplace <EntityMetaInfo> (eEditorController);
      cMetaInfo.packageId = "";

      registry.emplace <SubscriberInput> (eEditorController);

      auto& cInputController = registry.emplace <InputController> (eEditorController);

      auto& iEngineShutdown = cInputController.inputs["EngineShutdown"];
      iEngineShutdown.callbacks.insert("EngineShutdown");

      auto& iCameraControlOn = cInputController.inputs["EditorCameraControlOn"];
      iCameraControlOn.callbacks.insert("EditorCameraControlOn");

      auto& iEntitySelect = cInputController.inputs["EditorEntitySelect"];
      iEntitySelect.callbacks.insert("EditorEntitySelect");

      auto& iCursorPosX = cInputController.inputs["CursorPosX"];
      auto& iCursorPosY = cInputController.inputs["CursorPosY"];

      iCursorPosX.constraint.first = 1.0f;
      iCursorPosY.constraint.first = 1.0f;
    }

    auto& inputManager = registry.ctx().at <InputManager> ();

    if ( inputManager.axisAssigned("EngineShutdown") == false )
    {
      auto binding = std::make_shared <InputBindingRelative> ("-Key_Q", 0.0f);
      inputManager.assignBinding("EngineShutdown", binding);
    }

    if ( inputManager.axisAssigned("EditorEntitySelect") == false )
    {
      auto binding = std::make_shared <InputBindingRelative> ("+MouseButton_Left", 0.0f);
      inputManager.assignBinding("EditorEntitySelect", binding);
    }

    if ( inputManager.axisAssigned("EditorCameraControlOn") == false )
    {
      auto binding = std::make_shared <InputBindingRelative> ("+MouseButton_Right", 0.0f);
      inputManager.assignBinding("EditorCameraControlOn", binding);
    }

    if ( inputManager.axisAssigned("EditorCameraControlOff") == false )
    {
      auto binding = std::make_shared <InputBindingRelative> ("+Key_Escape", 0.0f);
      inputManager.assignBinding("EditorCameraControlOff", binding);
    }

    if ( inputManager.axisAssigned("EditorCameraFov") == false )
    {
      auto binding = std::make_shared <InputBindingRelative> ("+MouseWheel_Y");
      binding->sensitivity = -0.0005f;
      inputManager.assignBinding("EditorCameraFov", binding);

      binding = std::make_shared <InputBindingRelative> ("-MouseWheel_Y");
      binding->sensitivity = 0.0005f;
      inputManager.assignBinding("EditorCameraFov", binding);
    }

    if ( inputManager.axisAssigned("EditorCameraZoom") == false )
    {
      auto binding = std::make_shared <InputBindingRelative> ("+MouseWheel_Y");
      binding->sensitivity = -0.00001f;
      inputManager.assignBinding("EditorCameraZoom", binding);

      binding = std::make_shared <InputBindingRelative> ("-MouseWheel_Y");
      binding->sensitivity = 0.00001f;
      inputManager.assignBinding("EditorCameraZoom", binding);
    }
  };

  auto& callbackMgr = mRegistry.ctx().at <CallbackManager> ();

  callbackMgr.Register("ControlTranslateXRelative", controlTranslateXRelative);
  callbackMgr.Register("ControlTranslateYRelative", controlTranslateYRelative);
  callbackMgr.Register("ControlTranslateZRelative", controlTranslateZRelative);

  callbackMgr.Register("ControlPitchRelative", controlPitchRelative);
  callbackMgr.Register("ControlYawRelative", controlYawRelative);
  callbackMgr.Register("ControlRollRelative", controlRollRelative);

  callbackMgr.Register("EditorEntitySelect", editorEntitySelect);
  callbackMgr.Register("EditorCameraControlOn", editorCameraControlOn);
  callbackMgr.Register("EditorCameraFovControl", editorCameraFovControl);
  callbackMgr.Register("EditorCameraZoomControl", editorCameraZoomControl);

  callbackMgr.Register("MouseAutoCenterEnable", mouseAutoCenterEnable);
  callbackMgr.Register("MouseAutoCenterDisable", mouseAutoCenterDisable);

  callbackMgr.Register("MouseCursorHide", mouseCursorHide);
  callbackMgr.Register("MouseCursorShow", mouseCursorShow);

  callbackMgr.Register("EntityInputOn", entityInputOn);
  callbackMgr.Register("EntityInputOff", entityInputOff);

  callbackMgr.Register("EntityActivate", entityActivate);
  callbackMgr.Register("EntityDeactivate", entityDeactivate);

  callbackMgr.Register("EngineShutdown", engineShutdown);

  auto& systemMgr = mRegistry.ctx().at <SystemManager> ();

  using namespace cqde::systems;

  systemMgr.Register("EditorSystem",
                     EditorSystem,
                     System::Phase::Logic);

  systemMgr.Register("CullingSystem",
                     CullingSystem,
                     System::Phase::Render);
  systemMgr.Register("RenderSystem",
                     RenderSystem,
                     System::Phase::Render);
  systemMgr.Register("EditorEntityHighlightSystem",
                     EditorEntityHighlightSystem,
                     System::Phase::Render);

//  systemMgr.activate("CullingSystem");
  systemMgr.activate("RenderSystem");
  systemMgr.activate("EditorSystem");
  systemMgr.activate("EditorEntityHighlightSystem");
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
  using cqde::types::SystemManager;
  using cqde::types::TickCurrent;
  using cqde::types::System;

  const double dt = static_cast <double> (interval);
  const float cameraVelocity = 10.0f;

  auto& tick = mRegistry.ctx().at <TickCurrent> ();

  tick.ticksElapsed = ticks;
  tick.tickInterval = interval;

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

  mRegistry.ctx().at <SystemManager> ().execute(mRegistry,
                                                System::Phase::Render);

  frame.lastFrameTimepoint = TimeUtils::Now();
}
