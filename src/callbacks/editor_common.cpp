#include <cqde/callbacks/editor_common.hpp>

#include <cqde/math_helpers.hpp>

#include <cqde/types/EntityManager.hpp>
#include <cqde/types/SystemManager.hpp>
#include <cqde/types/CasqadiumEngine.hpp>

#include <cqde/types/ui/AssetManagerUi.hpp>
#include <cqde/types/ui/EntityManagerUi.hpp>
#include <cqde/types/ui/InputManagerUi.hpp>
#include <cqde/types/ui/PackageManagerUi.hpp>
#include <cqde/types/ui/PrefabManagerUi.hpp>
#include <cqde/types/ui/SystemManagerUi.hpp>
#include <cqde/types/ui/ViewportManagerUi.hpp>

#include <cqde/types/input/InputManager.hpp>
#include <cqde/types/input/InputBindingRelative.hpp>

#include <cqde/types/graphics/FrameReadback.hpp>

#include <cqde/components/Tag.hpp>
#include <cqde/components/Camera.hpp>
#include <cqde/components/Transform.hpp>
#include <cqde/components/SceneNode.hpp>
#include <cqde/components/EntityMetaInfo.hpp>
#include <cqde/components/InputController.hpp>
#include <cqde/components/SubscriberInput.hpp>
#include <cqde/components/SubscriberUpdate.hpp>
#include <cqde/components/CasqadiumEditorInternal.hpp>
#include <cqde/components/CasqadiumEditorCameraSettings.hpp>

#include <GLFW/glfw3.h>

#include <soloud.h>


namespace cqde::callbacks
{

void
editorModeEnable(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  registry.ctx().get <SoLoud::Soloud> ().stopAll();

  using ui::AssetManagerUi;
  if ( registry.ctx().contains <AssetManagerUi> () == false )
    registry.ctx().emplace <AssetManagerUi> ();

  using ui::ViewportManagerUi;
  if ( registry.ctx().contains <ViewportManagerUi> () == false )
    registry.ctx().emplace <ViewportManagerUi> ();

  using ui::EntityManagerUi;
  using types::EntityManager;
  if ( registry.ctx().contains <EntityManagerUi> () == false )
  {
    auto& entityManager = registry.ctx().get <EntityManager> ();
    registry.ctx().emplace <EntityManagerUi> (&entityManager);
  }

  using ui::InputManagerUi;
  using types::InputManager;
  if ( registry.ctx().contains <InputManagerUi> () == false )
  {
    auto& inputManager = registry.ctx().get <InputManager> ();
    registry.ctx().emplace <InputManagerUi> (&inputManager);
  }

  using ui::PackageManagerUi;
  using types::PackageManager;
  if ( registry.ctx().contains <PackageManagerUi> () == false )
  {
    auto& packageManager = registry.ctx().get <PackageManager> ();
    registry.ctx().emplace <PackageManagerUi> (&packageManager);
  }

  using ui::PrefabManagerUi;
  using types::PrefabManager;
  if ( registry.ctx().contains <PrefabManagerUi> () == false )
  {
    auto& prefabManager = registry.ctx().get <PrefabManager> ();
    registry.ctx().emplace <PrefabManagerUi> (&prefabManager);
  }

  using ui::SystemManagerUi;
  using types::SystemManager;

  auto& systemManager = registry.ctx().get <SystemManager> ();

  if ( registry.ctx().contains <SystemManagerUi> () == false )
    registry.ctx().emplace <SystemManagerUi> (&systemManager);

  editorCameraCreate(registry, args);
  editorControllerCreate(registry, args);

  registry.ctx().get <SystemManagerUi> ().init(registry);

  systemManager.deactivate();

  using Phase = types::System::Phase;

  for ( const auto& systemId : systemManager.systems(Phase::Editor) )
    systemManager.activate(systemId);

  using types::FrameReadbackQueue;
  auto& readbackQueue = registry.ctx().get <FrameReadbackQueue> ();
  readbackQueue.clear();
};

void
editorModeDisable(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using types::EntityManager;
  using types::SystemManager;
  using types::FrameReadbackQueue;
  using compos::CasqadiumEditorInternal;

  using Phase = types::System::Phase;

  registry.ctx().get <SoLoud::Soloud> ().stopAll();

  auto& entityManager = registry.ctx().get <EntityManager> ();
  auto& systemManager = registry.ctx().get <SystemManager> ();

  for ( const auto&& [entity] : registry.view <CasqadiumEditorInternal> ().each() )
    entityManager.removeLater(entity);

  for ( const auto& systemId : systemManager.systemsActive(Phase::Editor) )
    systemManager.deactivate(systemId);

  auto& readbackQueue = registry.ctx().get <FrameReadbackQueue> ();
  readbackQueue.clear();
};

void
editorModeToggle(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using compos::CasqadiumEditorInternal;

  if ( registry.storage <CasqadiumEditorInternal> ().empty() == true )
    return editorModeEnable(registry, args);

  editorModeDisable(registry, args);
};


void
editorCameraCreate(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using namespace compos;
  using namespace cqde::literals;
  using types::EntityManager;

  for ( const auto&& editorCamera
          : registry.view <Camera, CasqadiumEditorInternal> ().each() )
    return;

  auto& entityManager = registry.ctx().get <EntityManager> ();

  auto cameraId = "cqde_editor_camera"_id;

  if ( entityManager.get(cameraId) != entt::null )
    cameraId = entityManager.idGenerate(cameraId);

  const auto eCamera = entityManager.entityCreate(cameraId, registry);

  registry.emplace <SubscriberUpdate> (eCamera);
  registry.emplace <CasqadiumEditorInternal> (eCamera);
  registry.emplace <CasqadiumEditorCameraSettings> (eCamera).speed = 10.0f;

  auto& cMetaInfo = registry.emplace <EntityMetaInfo> (eCamera);
  cMetaInfo.packageId = "";

  auto& cCamera = registry.emplace <Camera> (eCamera);
  registry.emplace <Transform> (eCamera);
  registry.emplace <SceneNode> (eCamera);

//  cCamera.renderMode = Camera::RenderMode::Wireframe;

  auto& cInputController = registry.emplace <InputController> (eCamera);

  auto& iTranslateX = cInputController.axes["EditorCameraTranslateX"];
  auto& iTranslateY = cInputController.axes["EditorCameraTranslateY"];
  auto& iTranslateZ = cInputController.axes["EditorCameraTranslateZ"];

  iTranslateX.constraint = {1.0f, 0.0f};
  iTranslateY.constraint = {1.0f, 0.0f};
  iTranslateZ.constraint = {1.0f, 0.0f};

  iTranslateX.callbacks.push_back("EditorCameraTranslateXRelative");
  iTranslateY.callbacks.push_back("EditorCameraTranslateYRelative");
  iTranslateZ.callbacks.push_back("EditorCameraTranslateZRelative");

  auto& iPitch = cInputController.axes["EditorCameraPitch"];
  auto& iYaw = cInputController.axes["EditorCameraYaw"];
  auto& iRoll = cInputController.axes["EditorCameraRoll"];

  iPitch.constraint = {1.0f, 0.0f};
  iYaw.constraint = {1.0f, 0.0f};
  iRoll.constraint = {1.0f, 0.0f};

  iPitch.callbacks.push_back("ControlPitchRelative");
  iYaw.callbacks.push_back("ControlYawRelative");
  iRoll.callbacks.push_back("ControlRollRelative");

  auto& iCameraControlOff = cInputController.axes["EditorCameraControlOff"];

  iCameraControlOff.callbacks.push_back("EditorCameraControlOff");

  auto& iCameraSpeed = cInputController.axes["EditorCameraSpeedControl"];
  iCameraSpeed.constraint = {1.0f, 0.0f};
  iCameraSpeed.callbacks.push_back("EditorCameraSpeedControl");

  auto& iCameraZoom = cInputController.axes["EditorCameraZoom"];
  iCameraZoom.value = 1.0f;
  iCameraZoom.callbacks.push_back("EditorCameraZoomControl");
  iCameraZoom.constraint = { 1.0f, 0.0f };
}

void
editorControllerCreate(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using namespace compos;
  using namespace cqde::literals;
  using types::EntityManager;

  for ( const auto&& editorController
          : registry.view <InputController, CasqadiumEditorInternal> (entt::exclude <Camera>).each() )
    return;

  auto& entityManager = registry.ctx().get <EntityManager> ();

  auto controllerId = "cqde_editor_controller"_id;

  if ( entityManager.get(controllerId) != entt::null )
    controllerId = entityManager.idGenerate(controllerId);

  const auto eController = entityManager.entityCreate(controllerId, registry);

  auto& cMetaInfo = registry.emplace <EntityMetaInfo> (eController);
  cMetaInfo.packageId = "";

  registry.emplace <SubscriberInput> (eController);
  registry.emplace <SubscriberUpdate> (eController);
  registry.emplace <CasqadiumEditorInternal> (eController);

  auto& cInputController = registry.emplace <InputController> (eController);

  auto& iEngineShutdown = cInputController.axes["EngineShutdown"];
  iEngineShutdown.callbacks.push_back("EngineShutdown");

  auto& iEditorModeToggle = cInputController.axes["EditorModeToggle"];
  iEditorModeToggle.callbacks.push_back("EditorModeToggle");

  auto& iEditorGizmoModeTranslate = cInputController.axes["EditorGizmoModeTranslate"];
  iEditorGizmoModeTranslate.callbacks.push_back("EditorGizmoSetModeTranslate");

  auto& iEditorGizmoModeRotate = cInputController.axes["EditorGizmoModeRotate"];
  iEditorGizmoModeRotate.callbacks.push_back("EditorGizmoSetModeRotate");

  auto& iEditorGizmoModeScale = cInputController.axes["EditorGizmoModeScale"];
  iEditorGizmoModeScale.callbacks.push_back("EditorGizmoSetModeScale");

  auto& iEditorGizmoSpaceLocal = cInputController.axes["EditorGizmoSpaceLocal"];
  iEditorGizmoSpaceLocal.callbacks.push_back("EditorGizmoSetSpaceLocal");

  auto& iEditorGizmoSpaceWorld = cInputController.axes["EditorGizmoSpaceWorld"];
  iEditorGizmoSpaceWorld.callbacks.push_back("EditorGizmoSetSpaceWorld");

  auto& iCameraControlOn = cInputController.axes["EditorCameraControlOn"];
  iCameraControlOn.callbacks.push_back("EditorCameraControlOn");

  auto& iQuickSave = cInputController.axes["QuickSave"];
  iQuickSave.callbacks.push_back("QuickSave");

  auto& iQuickLoad = cInputController.axes["QuickLoad"];
  iQuickLoad.callbacks.push_back("QuickLoad");

  auto& iEntitySelect = cInputController.axes["EditorEntitySelect"];
  iEntitySelect.callbacks.push_back("EditorEntitySelect");

  auto& iEntityMultipleSelection = cInputController.axes["EditorEntityMultipleSelectionToggle"];
  iEntityMultipleSelection.callbacks.push_back("EditorEntityMultipleSelectionToggle");
}

void
editorBindingsAssign(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using types::InputManager;
  using types::InputBindingRelative;

  auto& inputManager = registry.ctx().get <InputManager> ();

  if ( inputManager.axisAssigned("EngineShutdown") == false )
  {
    auto binding = std::make_shared <InputBindingRelative> ("-Key_Q", 0.0f);
    inputManager.assignBinding("EngineShutdown", binding);
  }

  if ( inputManager.axisAssigned("EditorModeToggle") == false )
  {
    auto binding = std::make_shared <InputBindingRelative> ("-Key_F12", 0.0f);
    inputManager.assignBinding("EditorModeToggle", binding);
  }

  if ( inputManager.axisAssigned("EditorCameraTranslateX") == false )
  {
    auto binding = std::make_shared <InputBindingRelative> ("~Key_A", -1.0f);
    inputManager.assignBinding("EditorCameraTranslateX", binding);

    binding = std::make_shared <InputBindingRelative> ("~Key_D", 1.0f);
    inputManager.assignBinding("EditorCameraTranslateX", binding);
  }

  if ( inputManager.axisAssigned("EditorCameraTranslateY") == false )
  {
    auto binding = std::make_shared <InputBindingRelative> ("~Key_LeftShift", -1.0f);
    inputManager.assignBinding("EditorCameraTranslateY", binding);

    binding = std::make_shared <InputBindingRelative> ("~Key_Space", 1.0f);
    inputManager.assignBinding("EditorCameraTranslateY", binding);
  }

  if ( inputManager.axisAssigned("EditorCameraTranslateZ") == false )
  {
    auto binding = std::make_shared <InputBindingRelative> ("~Key_S", -1.0f);
    inputManager.assignBinding("EditorCameraTranslateZ", binding);

    binding = std::make_shared <InputBindingRelative> ("~Key_W", 1.0f);
    inputManager.assignBinding("EditorCameraTranslateZ", binding);
  }

  if ( inputManager.axisAssigned("EditorCameraPitch") == false )
  {
    auto binding = std::make_shared <InputBindingRelative> ("+MouseMove_Y", -0.1f);
    inputManager.assignBinding("EditorCameraPitch", binding);

    binding = std::make_shared <InputBindingRelative> ("-MouseMove_Y", 0.1f);
    inputManager.assignBinding("EditorCameraPitch", binding);
  }

  if ( inputManager.axisAssigned("EditorCameraYaw") == false )
  {
    auto binding = std::make_shared <InputBindingRelative> ("+MouseMove_X", -0.1f);
    inputManager.assignBinding("EditorCameraYaw", binding);

    binding = std::make_shared <InputBindingRelative> ("-MouseMove_X", 0.1f);
    inputManager.assignBinding("EditorCameraYaw", binding);
  }

  if ( inputManager.axisAssigned("EditorGizmoModeTranslate") == false )
  {
    auto binding = std::make_shared <InputBindingRelative> ("-Key_1", 0.0f);
    inputManager.assignBinding("EditorGizmoModeTranslate", binding);
  }

  if ( inputManager.axisAssigned("EditorGizmoModeRotate") == false )
  {
    auto binding = std::make_shared <InputBindingRelative> ("-Key_2", 0.0f);
    inputManager.assignBinding("EditorGizmoModeRotate", binding);
  }

  if ( inputManager.axisAssigned("EditorGizmoModeScale") == false )
  {
    auto binding = std::make_shared <InputBindingRelative> ("-Key_3", 0.0f);
    inputManager.assignBinding("EditorGizmoModeScale", binding);
  }

  if ( inputManager.axisAssigned("EditorGizmoSpaceLocal") == false )
  {
    auto binding = std::make_shared <InputBindingRelative> ("-Key_R", 0.0f);
    inputManager.assignBinding("EditorGizmoSpaceLocal", binding);
  }

  if ( inputManager.axisAssigned("EditorGizmoSpaceWorld") == false )
  {
    auto binding = std::make_shared <InputBindingRelative> ("-Key_F", 0.0f);
    inputManager.assignBinding("EditorGizmoSpaceWorld", binding);
  }

  if ( inputManager.axisAssigned("EditorEntitySelect") == false )
  {
    auto binding = std::make_shared <InputBindingRelative> ("-MouseButton_Left", 0.0f);
    inputManager.assignBinding("EditorEntitySelect", binding);
  }

  if ( inputManager.axisAssigned("EditorEntityMultipleSelectionToggle") == false )
  {
    auto binding = std::make_shared <InputBindingRelative> ("+Key_LeftCtrl", 0.0f);
    inputManager.assignBinding("EditorEntityMultipleSelectionToggle", binding);

    binding = std::make_shared <InputBindingRelative> ("-Key_LeftCtrl", 0.0f);
    inputManager.assignBinding("EditorEntityMultipleSelectionToggle", binding);
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

  if ( inputManager.axisAssigned("EditorCameraSpeedControl") == false )
  {
    auto binding = std::make_shared <InputBindingRelative> ("+MouseWheel_Y", 0.5f);
    inputManager.assignBinding("EditorCameraSpeedControl", binding);

    binding = std::make_shared <InputBindingRelative> ("-MouseWheel_Y", -0.5f);
    inputManager.assignBinding("EditorCameraSpeedControl", binding);
  }

  if ( inputManager.axisAssigned("EditorCameraFov") == false )
  {
    auto binding = std::make_shared <InputBindingRelative> ("+MouseWheel_Y", -1.0f);
    inputManager.assignBinding("EditorCameraFov", binding);

    binding = std::make_shared <InputBindingRelative> ("-MouseWheel_Y", 1.0f);
    inputManager.assignBinding("EditorCameraFov", binding);
  }

  if ( inputManager.axisAssigned("EditorCameraZoom") == false )
  {
    auto binding = std::make_shared <InputBindingRelative> ("~Key_W", -1.0f);
    inputManager.assignBinding("EditorCameraZoom", binding);

    binding = std::make_shared <InputBindingRelative> ("~Key_S", 1.0f);
    inputManager.assignBinding("EditorCameraZoom", binding);
  }

  if ( inputManager.axisAssigned("QuickSave") == false )
  {
    auto binding = std::make_shared <InputBindingRelative> ("-Key_F1", 0.0f);
    inputManager.assignBinding("QuickSave", binding);
  }

  if ( inputManager.axisAssigned("QuickLoad") == false )
  {
    auto binding = std::make_shared <InputBindingRelative> ("-Key_F2", 0.0f);
    inputManager.assignBinding("QuickLoad", binding);
  }
}

void
editorEntitySelect(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using types::EntityManager;
  using types::CasqadiumEngine;
  using types::FrameReadbackQueue;
  using types::FrameReadbackResult;
  using CallbackArgs = types::CallbackManager::CallbackArgs;

  using ui::EntityManagerUi;
  using ui::ViewportManagerUi;

  using compos::Tag;
  using compos::Camera;
  using compos::InputController;
  using compos::SubscriberInput;
  using compos::CasqadiumEditorInternal;

  const auto& engine = *registry.ctx().get <CasqadiumEngine*> ();

  glm::dvec2 cursorPos {};
  glm::ivec2 windowPos {};

  if ( engine.hoveredWindow() == nullptr )
    return;

  glfwGetCursorPos(engine.hoveredWindow(), &cursorPos.x, &cursorPos.y);
  glfwGetWindowPos(engine.hoveredWindow(), &windowPos.x, &windowPos.y);

  auto& readbackQueue = registry.ctx().get <FrameReadbackQueue> ();
  auto& viewportManagerUi = registry.ctx().get <const ViewportManagerUi> ();
  auto& entityManagerUi = registry.ctx().get <EntityManagerUi> ();

  const bool multipleSelectionEnabled = entityManagerUi.entitiesMultipleSelection();

  for ( const auto& viewport : viewportManagerUi.viewports() )
  {
    if ( viewportManagerUi.mouseOverViewport(viewport.camera.id) == false )
      continue;

    const glm::u16vec2 pos
    {
      windowPos.x + cursorPos.x - viewport.pos.x,
      viewport.framebuffer.size.y - (windowPos.y + cursorPos.y - viewport.pos.y),
    };

    readbackQueue.push(
      viewport.framebuffer.fbo,
      pos, {1u, 1u},
    [&entityManagerUi, multipleSelectionEnabled]
    ( entt::registry& registry,
      const CallbackArgs& args )
    {
      const auto result =
        std::any_cast <FrameReadbackResult> (args.at(0));

      const auto entity = static_cast <entt::entity> (result.data.front());

      if ( multipleSelectionEnabled == false )
        entityManagerUi.entitiesDeselect();

      if ( entity != entt::null )
      {
        if ( multipleSelectionEnabled == true &&
             entityManagerUi.entitySelected(entity) == true )
            return entityManagerUi.entityDeselect(entity);

        return entityManagerUi.entitySelect(entity);
      }
    });
  }
};

} // namespace cqde::callbacks
