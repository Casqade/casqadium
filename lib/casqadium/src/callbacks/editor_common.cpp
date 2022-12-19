#include <cqde/callbacks/editor_common.hpp>

#include <cqde/math_helpers.hpp>

#include <cqde/types/EntityManager.hpp>
#include <cqde/types/ui/EntityManagerUi.hpp>
#include <cqde/types/ui/ViewportManagerUi.hpp>

#include <cqde/types/input/InputManager.hpp>
#include <cqde/types/input/InputBindingRelative.hpp>

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


namespace cqde::callbacks
{

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

  auto& entityManager = registry.ctx().at <EntityManager> ();

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

  cCamera.renderMode = Camera::RenderMode::Wireframe;

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
  iCameraZoom.value = 0.01;
  iCameraZoom.callbacks.push_back("EditorCameraZoomControl");
  iCameraZoom.constraint = {  std::numeric_limits <float>::min(),
                              std::numeric_limits <float>::max() };
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

  auto& entityManager = registry.ctx().at <EntityManager> ();

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

  auto& iCursorPosX = cInputController.axes["CursorPosX"];
  auto& iCursorPosY = cInputController.axes["CursorPosY"];

  iCursorPosX.constraint.first = 1.0f;
  iCursorPosY.constraint.first = 1.0f;
}

void
editorBindingsAssign(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using types::InputManager;
  using types::InputBindingRelative;

  auto& inputManager = registry.ctx().at <InputManager> ();

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
    auto binding = std::make_shared <InputBindingRelative> ("~Key_Shift", -1.0f);
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
    auto binding = std::make_shared <InputBindingRelative> ("+MouseMove_Y", -10.0f);
    inputManager.assignBinding("EditorCameraPitch", binding);

    binding = std::make_shared <InputBindingRelative> ("-MouseMove_Y", 10.0f);
    inputManager.assignBinding("EditorCameraPitch", binding);
  }

  if ( inputManager.axisAssigned("EditorCameraYaw") == false )
  {
    auto binding = std::make_shared <InputBindingRelative> ("+MouseMove_X", -10.0f);
    inputManager.assignBinding("EditorCameraYaw", binding);

    binding = std::make_shared <InputBindingRelative> ("-MouseMove_X", 10.0f);
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
    auto binding = std::make_shared <InputBindingRelative> ("+Key_Ctrl", 0.0f);
    inputManager.assignBinding("EditorEntityMultipleSelectionToggle", binding);

    binding = std::make_shared <InputBindingRelative> ("-Key_Ctrl", 0.0f);
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
    auto binding = std::make_shared <InputBindingRelative> ("+MouseWheel_Y", 1.0f);
    inputManager.assignBinding("EditorCameraSpeedControl", binding);

    binding = std::make_shared <InputBindingRelative> ("-MouseWheel_Y", -1.0f);
    inputManager.assignBinding("EditorCameraSpeedControl", binding);
  }

  if ( inputManager.axisAssigned("EditorCameraFov") == false )
  {
    auto binding = std::make_shared <InputBindingRelative> ("+MouseWheel_Y", -0.0005f);
    inputManager.assignBinding("EditorCameraFov", binding);

    binding = std::make_shared <InputBindingRelative> ("-MouseWheel_Y", 0.0005f);
    inputManager.assignBinding("EditorCameraFov", binding);
  }

  if ( inputManager.axisAssigned("EditorCameraZoom") == false )
  {
    auto binding = std::make_shared <InputBindingRelative> ("~Key_W", -0.0001f);
    inputManager.assignBinding("EditorCameraZoom", binding);

    binding = std::make_shared <InputBindingRelative> ("~Key_S", 0.0001f);
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

  using ui::EntityManagerUi;
  using ui::ViewportManagerUi;

  using compos::Tag;
  using compos::Camera;
  using compos::InputController;
  using compos::SubscriberInput;
  using compos::CasqadiumEditorInternal;

  const auto cController = std::any_cast <InputController*> (args.at(1));

  const auto cursorPosX = cController->axes.find("CursorPosX");
  const auto cursorPosY = cController->axes.find("CursorPosY");

  if ( cursorPosX == cController->axes.end() ||
       cursorPosY == cController->axes.end() )
    return;

  const glm::vec2 cursorPos
  {
    cursorPosX->second.value,
    cursorPosY->second.value,
  };

  auto& viewportManagerUi = registry.ctx().at <const ViewportManagerUi> ();
  auto& entityManagerUi = registry.ctx().at <EntityManagerUi> ();

  const bool multipleSelectionEnabled = entityManagerUi.entitiesMultipleSelection();

  for ( const auto&& [eCamera, cTag, cCamera]
          : registry.view <Tag, Camera, CasqadiumEditorInternal> ().each() )
  {
    if ( viewportManagerUi.mouseOverViewport(cTag.id) == false )
      continue;

    if ( registry.all_of <SubscriberInput> (eCamera) == true )
      return;

    for ( auto iter = cCamera.zBuffer.rbegin();
          iter != cCamera.zBuffer.rend();
          ++iter )
    {
      const auto& [vBuf, entity] = *iter;

      if ( pointInRect( cursorPos, boundingBox(vBuf.vertices) ) == false )
        continue;

      if ( multipleSelectionEnabled == false )
      {
        if ( entityManagerUi.entitySelected(entity) == true )
          continue;

        entityManagerUi.entitiesDeselect();
        return entityManagerUi.entitySelect(entity);
      }

      if ( entityManagerUi.entitySelected(entity) == true )
        return entityManagerUi.entityDeselect(entity);

      return entityManagerUi.entitySelect(entity);
    }

    if ( multipleSelectionEnabled == false )
      entityManagerUi.entitiesDeselect();
  }
};

} // namespace cqde::callbacks
