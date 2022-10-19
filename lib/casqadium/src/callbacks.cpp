#include <cqde/callbacks.hpp>

#include <cqde/types/TickCurrent.hpp>

#include <cqde/types/CallbackManager.hpp>
#include <cqde/types/EntityManager.hpp>
#include <cqde/types/SystemManager.hpp>
#include <cqde/types/SnapshotManager.hpp>
#include <cqde/types/UserManager.hpp>

#include <cqde/types/input/InputManager.hpp>
#include <cqde/types/input/InputBindingRelative.hpp>


#include <cqde/types/ui/AssetManagerUi.hpp>
#include <cqde/types/ui/EntityManagerUi.hpp>
#include <cqde/types/ui/InputManagerUi.hpp>
#include <cqde/types/ui/PackageManagerUi.hpp>
#include <cqde/types/ui/PrefabManagerUi.hpp>
#include <cqde/types/ui/SystemManagerUi.hpp>
#include <cqde/types/ui/ViewportManagerUi.hpp>

#include <cqde/types/assets/AudioAssetManager.hpp>
#include <cqde/types/assets/FontAssetManager.hpp>
#include <cqde/types/assets/GeometryAssetManager.hpp>
#include <cqde/types/assets/TextStringAssetManager.hpp>
#include <cqde/types/assets/TextureAssetManager.hpp>

#include <cqde/components/Tag.hpp>
#include <cqde/components/Camera.hpp>
#include <cqde/components/Transform.hpp>
#include <cqde/components/SceneNode.hpp>
#include <cqde/components/EntityMetaInfo.hpp>
#include <cqde/components/SubscriberInput.hpp>
#include <cqde/components/SubscriberUpdate.hpp>
#include <cqde/components/InputController.hpp>
#include <cqde/components/CasqadiumEditorInternal.hpp>
#include <cqde/components/CasqadiumEditorCameraSettings.hpp>
#include <cqde/components/WantsMouseCentered.hpp>
#include <cqde/components/WantsMouseHidden.hpp>

#include <cqde/components/assets/AudioAssetLoadList.hpp>
#include <cqde/components/assets/AudioAssetUnloadList.hpp>
#include <cqde/components/assets/FontAssetLoadList.hpp>
#include <cqde/components/assets/FontAssetUnloadList.hpp>
#include <cqde/components/assets/GeometryAssetLoadList.hpp>
#include <cqde/components/assets/GeometryAssetUnloadList.hpp>
#include <cqde/components/assets/TextStringAssetLoadList.hpp>
#include <cqde/components/assets/TextStringAssetUnloadList.hpp>
#include <cqde/components/assets/TextureAssetLoadList.hpp>
#include <cqde/components/assets/TextureAssetUnloadList.hpp>

#include <cqde/file_helpers.hpp>
#include <cqde/json_helpers.hpp>
#include <cqde/math_helpers.hpp>
#include <cqde/util/logger.hpp>

#include <olcPGE/olcPixelGameEngine.hpp>

#include <entt/entity/registry.hpp>

#include <json/writer.h>


namespace cqde::callbacks
{

void
mouseAutoCenterEnable(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  olc::platform->ptrPGE->SetKeepMouseCentered(true);
};

void
mouseAutoCenterDisable(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  olc::platform->ptrPGE->SetKeepMouseCentered(false);
};

void
mouseAutoCenterToggle(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  if ( olc::platform->ptrPGE->GetKeepMouseCentered() == true )
    return olc::platform->ptrPGE->SetKeepMouseCentered(false);

  olc::platform->ptrPGE->SetKeepMouseCentered(true);
};

void
mouseCursorHide(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  olc::platform->ptrPGE->SetMouseCursor(olc::Mouse::Cursor{});
};

void
mouseCursorShow(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  olc::platform->ptrPGE->ResetMouseCursor();
};

void
mouseGrabToggle(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using compos::InputController;
  using compos::SubscriberInput;

  if ( olc::platform->ptrPGE->GetKeepMouseCentered() == true )
  {
    mouseAutoCenterDisable(registry, args);
    mouseCursorShow(registry, args);

    return;
  }

  mouseAutoCenterEnable(registry, args);
  mouseCursorHide(registry, args);
}

void
editorModeEnable(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
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
    auto& entityManager = registry.ctx().at <EntityManager> ();
    registry.ctx().emplace <EntityManagerUi> (&entityManager);
  }

  using ui::InputManagerUi;
  using types::InputManager;
  if ( registry.ctx().contains <InputManagerUi> () == false )
  {
    auto& inputManager = registry.ctx().at <InputManager> ();
    registry.ctx().emplace <InputManagerUi> (&inputManager);
  }

  using ui::PackageManagerUi;
  using types::PackageManager;
  if ( registry.ctx().contains <PackageManagerUi> () == false )
  {
    auto& packageManager = registry.ctx().at <PackageManager> ();
    registry.ctx().emplace <PackageManagerUi> (&packageManager);
  }

  using ui::PrefabManagerUi;
  using types::PrefabManager;
  if ( registry.ctx().contains <PrefabManagerUi> () == false )
  {
    auto& prefabManager = registry.ctx().at <PrefabManager> ();
    registry.ctx().emplace <PrefabManagerUi> (&prefabManager);
  }

  using ui::SystemManagerUi;
  using types::SystemManager;

  auto& systemManager = registry.ctx().at <SystemManager> ();

  if ( registry.ctx().contains <SystemManagerUi> () == false )
    registry.ctx().emplace <SystemManagerUi> (&systemManager);

  editorCameraCreate(registry, args);
  editorControllerCreate(registry, args);

  registry.ctx().at <SystemManagerUi> ().init(registry);

  systemManager.deactivate();

  using Phase = types::System::Phase;

  for ( const auto& systemId : systemManager.systems(Phase::Editor) )
    systemManager.activate(systemId);
};

void
editorModeDisable(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using types::EntityManager;
  using types::SystemManager;
  using compos::CasqadiumEditorInternal;

  using Phase = types::System::Phase;

  auto& entityManager = registry.ctx().at <EntityManager> ();
  auto& systemManager = registry.ctx().at <SystemManager> ();

  for ( const auto&& [entity] : registry.view <CasqadiumEditorInternal> ().each() )
    entityManager.removeLater(entity);

  for ( const auto& systemId : systemManager.systemsActive(Phase::Editor) )
    systemManager.deactivate(systemId);
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
editorGizmoSetModeTranslate(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using ui::ViewportManagerUi;
  auto& viewportManagerUi = registry.ctx().at <ViewportManagerUi> ();
  viewportManagerUi.setGizmoOperation(ImGuizmo::OPERATION::TRANSLATE);
}

void
editorGizmoSetModeRotate(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using ui::ViewportManagerUi;
  auto& viewportManagerUi = registry.ctx().at <ViewportManagerUi> ();
  viewportManagerUi.setGizmoOperation(ImGuizmo::OPERATION::ROTATE);
}

void
editorGizmoSetModeScale(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using ui::ViewportManagerUi;
  auto& viewportManagerUi = registry.ctx().at <ViewportManagerUi> ();
  viewportManagerUi.setGizmoOperation(ImGuizmo::OPERATION::SCALE);
  viewportManagerUi.setGizmoSpace(ImGuizmo::MODE::LOCAL);
}

void
editorGizmoSetSpaceLocal(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using ui::ViewportManagerUi;
  auto& viewportManagerUi = registry.ctx().at <ViewportManagerUi> ();
  viewportManagerUi.setGizmoSpace(ImGuizmo::MODE::LOCAL);
}

void
editorGizmoSetSpaceWorld(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using ui::ViewportManagerUi;
  auto& viewportManagerUi = registry.ctx().at <ViewportManagerUi> ();
  viewportManagerUi.setGizmoSpace(ImGuizmo::MODE::WORLD);
}

void
editorEntityMultipleSelectionEnable(
  entt::registry& registry,
  const std::vector <std::any>& args )
  {
  using ui::EntityManagerUi;

  registry.ctx().at <EntityManagerUi> ().setEntitiesMultipleSelection(true);
};

void
editorEntityMultipleSelectionDisable(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using ui::EntityManagerUi;

  registry.ctx().at <EntityManagerUi> ().setEntitiesMultipleSelection(false);
};

void
editorEntityMultipleSelectionToggle(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using ui::EntityManagerUi;

  auto& entityManagerUi = registry.ctx().at <EntityManagerUi> ();

  const bool multipleSelectionEnabled = entityManagerUi.entitiesMultipleSelection();

  entityManagerUi.setEntitiesMultipleSelection( !multipleSelectionEnabled );
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

  iTranslateX.callbacks.insert("EditorCameraTranslateXRelative");
  iTranslateY.callbacks.insert("EditorCameraTranslateYRelative");
  iTranslateZ.callbacks.insert("EditorCameraTranslateZRelative");

  auto& iPitch = cInputController.axes["EditorCameraPitch"];
  auto& iYaw = cInputController.axes["EditorCameraYaw"];
  auto& iRoll = cInputController.axes["EditorCameraRoll"];

  iPitch.constraint = {1.0f, 0.0f};
  iYaw.constraint = {1.0f, 0.0f};
  iRoll.constraint = {1.0f, 0.0f};

  iPitch.callbacks.insert("ControlPitchRelative");
  iYaw.callbacks.insert("ControlYawRelative");
  iRoll.callbacks.insert("ControlRollRelative");

  auto& iCameraControlOff = cInputController.axes["EditorCameraControlOff"];

  iCameraControlOff.callbacks.insert("EditorCameraControlOff");

  auto& iCameraSpeed = cInputController.axes["EditorCameraSpeedControl"];
  iCameraSpeed.constraint = {1.0f, 0.0f};
  iCameraSpeed.callbacks.insert("EditorCameraSpeedControl");

  auto& iCameraZoom = cInputController.axes["EditorCameraZoom"];
  iCameraZoom.value = 0.01;
  iCameraZoom.callbacks.insert("EditorCameraZoomControl");
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
  registry.emplace <CasqadiumEditorInternal> (eController);

  auto& cInputController = registry.emplace <InputController> (eController);

  auto& iEngineShutdown = cInputController.axes["EngineShutdown"];
  iEngineShutdown.callbacks.insert("EngineShutdown");

  auto& iEditorModeToggle = cInputController.axes["EditorModeToggle"];
  iEditorModeToggle.callbacks.insert("EditorModeToggle");

  auto& iEditorGizmoModeTranslate = cInputController.axes["EditorGizmoModeTranslate"];
  iEditorGizmoModeTranslate.callbacks.insert("EditorGizmoSetModeTranslate");

  auto& iEditorGizmoModeRotate = cInputController.axes["EditorGizmoModeRotate"];
  iEditorGizmoModeRotate.callbacks.insert("EditorGizmoSetModeRotate");

  auto& iEditorGizmoModeScale = cInputController.axes["EditorGizmoModeScale"];
  iEditorGizmoModeScale.callbacks.insert("EditorGizmoSetModeScale");

  auto& iEditorGizmoSpaceLocal = cInputController.axes["EditorGizmoSpaceLocal"];
  iEditorGizmoSpaceLocal.callbacks.insert("EditorGizmoSetSpaceLocal");

  auto& iEditorGizmoSpaceWorld = cInputController.axes["EditorGizmoSpaceWorld"];
  iEditorGizmoSpaceWorld.callbacks.insert("EditorGizmoSetSpaceWorld");

  auto& iCameraControlOn = cInputController.axes["EditorCameraControlOn"];
  iCameraControlOn.callbacks.insert("EditorCameraControlOn");

  auto& iQuickSave = cInputController.axes["QuickSave"];
  iQuickSave.callbacks.insert("QuickSave");

  auto& iQuickLoad = cInputController.axes["QuickLoad"];
  iQuickLoad.callbacks.insert("QuickLoad");

  auto& iEntitySelect = cInputController.axes["EditorEntitySelect"];
  iEntitySelect.callbacks.insert("EditorEntitySelect");

  auto& iEntityMultipleSelection = cInputController.axes["EditorEntityMultipleSelectionToggle"];
  iEntityMultipleSelection.callbacks.insert("EditorEntityMultipleSelectionToggle");

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
editorCameraControlOn(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using types::EntityManager;
  using ui::ViewportManagerUi;
  using namespace compos;

  for ( const auto&& [eCamera, cTag, cCamera]
          : registry.view <Tag, Camera, CasqadiumEditorInternal> ().each() )
  {
    if ( registry.ctx().at <ViewportManagerUi> ().mouseOverViewport(cTag.id) == false )
      continue;

    if ( registry.all_of <SubscriberInput> (eCamera) == true )
      continue;

    entityInputOn(registry, {eCamera});

    registry.emplace_or_replace <WantsMouseCentered> (eCamera);
    registry.emplace_or_replace <WantsMouseHidden> (eCamera);

    return;
  }
};

void
editorCameraControlOff(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using types::EntityManager;
  using ui::ViewportManagerUi;
  using namespace compos;

  const auto eCamera = std::any_cast <entt::entity> (args.at(0));

  entityInputOff(registry, args);

  registry.remove <WantsMouseCentered> (eCamera);
  registry.remove <WantsMouseHidden> (eCamera);
};

void
editorCameraFovControl(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using compos::Camera;
  using compos::InputController;
  using types::ControlAxis;

  const auto eCamera = std::any_cast <entt::entity> (args.at(0));
  const auto axis = std::any_cast <ControlAxis*> (args.at(2));

  auto& cCamera = registry.get <Camera> (eCamera);

  if ( cCamera.projectionType == Camera::Projection::Perspective )
    cCamera.fov = axis->value;
};

void
editorCameraZoomControl(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using compos::Camera;
  using compos::InputController;
  using types::ControlAxis;

  const auto eCamera = std::any_cast <entt::entity> (args.at(0));
  const auto axis = std::any_cast <ControlAxis*> (args.at(2));

  auto& cCamera = registry.get <Camera> (eCamera);

  if ( cCamera.projectionType == Camera::Projection::Orthographic )
    cCamera.zoom = axis->value;
};

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

  const auto& viewportManagerUi = registry.ctx().at <ViewportManagerUi> ();

  for ( const auto&& [eCamera, cTag, cCamera]
          : registry.view <Tag, Camera, CasqadiumEditorInternal> ().each() )
  {
    if ( registry.ctx().at <ViewportManagerUi> ().mouseOverViewport(cTag.id) == false )
      continue;

    const glm::vec2 cursorPos
    {
      cController->axes["CursorPosX"].value,
      cController->axes["CursorPosY"].value,
    };

    auto& entityManagerUi = registry.ctx().at <EntityManagerUi> ();

    if ( registry.all_of <SubscriberInput> (eCamera) == true )
      return;

    const bool multipleSelectionEnabled = entityManagerUi.entitiesMultipleSelection();

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

void
quickSave(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using types::SnapshotManager;

  SnapshotManager::Write(registry, "quick.json");
};

void
quickLoad(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using types::CallbackManager;
  using types::SnapshotManager;
  using types::UserManager;

  registry.ctx().at <CallbackManager> ().executeLater(
  [] (  entt::registry& registry ,
        const CallbackManager::CallbackArgs& )
  {
    try
    {
      const auto snapshotPath = registry.ctx().at <UserManager> ().snapshotsRoot();
      SnapshotManager::Load(registry, snapshotPath / "quick.json");
    }
    catch ( const std::exception& e )
    {
      LOG_ERROR(e.what());
    }
  });
};

void
entityUpdateOn(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using compos::SubscriberUpdate;

  const auto entity = std::any_cast <entt::entity> (args.at(0));
  registry.emplace_or_replace <SubscriberUpdate> (entity);
};

void
entityUpdateOff(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using compos::SubscriberUpdate;

  const auto entity = std::any_cast <entt::entity> (args.at(0));
  registry.remove <SubscriberUpdate> (entity);
};

void
entityUpdateToggle(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using compos::SubscriberUpdate;

  const auto entity = std::any_cast <entt::entity> (args.at(0));

  if ( registry.all_of <SubscriberUpdate> (entity) == true )
    return entityUpdateOff(registry, args);

  entityUpdateOn(registry, args);
};

void
entityInputOn(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using compos::SubscriberInput;

  const auto entity = std::any_cast <entt::entity> (args.at(0));
  registry.emplace_or_replace <SubscriberInput> (entity);
};

void
entityInputOff(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using compos::SubscriberInput;

  const auto entity = std::any_cast <entt::entity> (args.at(0));
  registry.remove <SubscriberInput> (entity);
};

void
entityInputToggle(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using compos::SubscriberInput;

  const auto entity = std::any_cast <entt::entity> (args.at(0));

  if ( registry.all_of <SubscriberInput> (entity) == true )
    return entityInputOff(registry, args);

  entityInputOn(registry, args);
};

void
editorCameraSpeedControl(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using compos::Transform;
  using compos::CasqadiumEditorCameraSettings;
  using types::ControlAxis;
  using types::TickCurrent;

  const auto entity = std::any_cast <entt::entity> (args.at(0));
  const auto axis = std::any_cast <ControlAxis*> (args.at(2));

  const auto& tick = registry.ctx().at <TickCurrent> ();
  const auto ticks = tick.ticksElapsed;
  const auto elapsed = tick.tickInterval;

  const float dt = ticks * static_cast <double> (elapsed);

  auto& cSettings = registry.get <CasqadiumEditorCameraSettings> (entity);
  cSettings.speed += axis->value * dt;

  axis->value = 0.0f;
};

void
editorCameraTranslateXRelative(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using compos::Transform;
  using compos::CasqadiumEditorCameraSettings;
  using types::ControlAxis;
  using types::TickCurrent;

  const auto entity = std::any_cast <entt::entity> (args.at(0));
  const auto axis = std::any_cast <ControlAxis*> (args.at(2));

  const auto& tick = registry.ctx().at <TickCurrent> ();
  const auto ticks = tick.ticksElapsed;
  const auto elapsed = tick.tickInterval;

  const float dt = ticks * static_cast <double> (elapsed);

  auto&& [cTransform, cSettings] = registry.get <Transform, CasqadiumEditorCameraSettings> (entity);

  cTransform.translation += cTransform.right() * axis->value * cSettings.speed * dt;

  axis->value = 0.0f;
};

void
editorCameraTranslateYRelative(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using compos::Transform;
  using compos::CasqadiumEditorCameraSettings;
  using types::ControlAxis;
  using types::TickCurrent;

  const auto entity = std::any_cast <entt::entity> (args.at(0));
  const auto axis = std::any_cast <ControlAxis*> (args.at(2));

  const auto& tick = registry.ctx().at <TickCurrent> ();
  const auto ticks = tick.ticksElapsed;
  const auto elapsed = tick.tickInterval;

  const float dt = ticks * static_cast <double> (elapsed);

  auto&& [cTransform, cSettings] = registry.get <Transform, CasqadiumEditorCameraSettings> (entity);

  cTransform.translation += cTransform.up() * axis->value * cSettings.speed * dt;

  axis->value = 0.0f;
};

void
editorCameraTranslateZRelative(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using compos::Camera;
  using compos::Transform;
  using compos::CasqadiumEditorCameraSettings;
  using types::ControlAxis;
  using types::TickCurrent;

  const auto entity = std::any_cast <entt::entity> (args.at(0));
  const auto axis = std::any_cast <ControlAxis*> (args.at(2));

  const auto& tick = registry.ctx().at <TickCurrent> ();
  const auto ticks = tick.ticksElapsed;
  const auto elapsed = tick.tickInterval;

  const float dt = ticks * static_cast <double> (elapsed);

  auto&& [cTransform, cCamera, cSettings] = registry.get <Transform, Camera, CasqadiumEditorCameraSettings> (entity);

  if ( cCamera.projectionType == Camera::Projection::Perspective )
    cTransform.translation += cTransform.front() * axis->value * cSettings.speed * dt;

  axis->value = 0.0f;
};

void
controlTranslateXRelative(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using compos::Transform;
  using types::ControlAxis;
  using types::TickCurrent;

  const auto entity = std::any_cast <entt::entity> (args.at(0));
  const auto axis = std::any_cast <ControlAxis*> (args.at(2));

  const auto& tick = registry.ctx().at <TickCurrent> ();
  const auto ticks = tick.ticksElapsed;
  const auto elapsed = tick.tickInterval;

  const float dt = ticks * static_cast <double> (elapsed);

  auto& cTransform = registry.get <Transform> (entity);

  cTransform.translation += cTransform.right() * axis->value * dt;

  axis->value = 0.0f;
};

void
controlTranslateYRelative(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using compos::Transform;
  using types::ControlAxis;
  using types::TickCurrent;

  const auto entity = std::any_cast <entt::entity> (args.at(0));
  const auto axis = std::any_cast <ControlAxis*> (args.at(2));

  const auto& tick = registry.ctx().at <TickCurrent> ();
  const auto ticks = tick.ticksElapsed;
  const auto elapsed = tick.tickInterval;

  const float dt = ticks * static_cast <double> (elapsed);

  auto& cTransform = registry.get <Transform> (entity);

  cTransform.translation += cTransform.up() * axis->value * dt;

  axis->value = 0.0f;
};

void
controlTranslateZRelative(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using compos::Transform;
  using types::ControlAxis;
  using types::TickCurrent;

  const auto entity = std::any_cast <entt::entity> (args.at(0));
  const auto axis = std::any_cast <ControlAxis*> (args.at(2));

  const auto& tick = registry.ctx().at <TickCurrent> ();
  const auto ticks = tick.ticksElapsed;
  const auto elapsed = tick.tickInterval;

  const float dt = ticks * static_cast <double> (elapsed);

  auto& cTransform = registry.get <Transform> (entity);

  cTransform.translation += cTransform.front() * axis->value * dt;

  axis->value = 0.0f;
};

void
controlPitchRelative(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using compos::Transform;
  using types::ControlAxis;
  using types::TickCurrent;

  const auto entity = std::any_cast <entt::entity> (args.at(0));
  const auto axis = std::any_cast <ControlAxis*> (args.at(2));

  const auto& tick = registry.ctx().at <TickCurrent> ();
  const auto ticks = tick.ticksElapsed;
  const auto elapsed = tick.tickInterval;

  const float dt = ticks * static_cast <double> (elapsed);

  auto& cTransform = registry.get <Transform> (entity);

  const auto angle = glm::angleAxis(glm::radians(axis->value * dt),
                                    glm::vec3 {1.0f, 0.0f, 0.0f});

  const auto orientationPrev = cTransform.orientation;

  cTransform.orientation = glm::normalize(cTransform.orientation * angle);

  if ( cTransform.up().y < 0.0f )
    cTransform.orientation = orientationPrev;

  axis->value = 0.0f;
};

void
controlYawRelative(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using compos::Transform;
  using types::ControlAxis;
  using types::TickCurrent;

  const auto entity = std::any_cast <entt::entity> (args.at(0));
  const auto axis = std::any_cast <ControlAxis*> (args.at(2));

  const auto& tick = registry.ctx().at <TickCurrent> ();
  const auto ticks = tick.ticksElapsed;
  const auto elapsed = tick.tickInterval;

  const float dt = ticks * static_cast <double> (elapsed);

  auto& cTransform = registry.get <Transform> (entity);

  const auto angle = glm::angleAxis(glm::radians(axis->value * dt),
                                    glm::vec3 {0.0f, 1.0f, 0.0f});

  cTransform.orientation = glm::normalize(angle * cTransform.orientation);

  axis->value = 0.0f;
};

void
controlRollRelative(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using compos::Transform;
  using types::ControlAxis;
  using types::TickCurrent;

  const auto entity = std::any_cast <entt::entity> (args.at(0));
  const auto axis = std::any_cast <ControlAxis*> (args.at(2));

  const auto& tick = registry.ctx().at <TickCurrent> ();
  const auto ticks = tick.ticksElapsed;
  const auto elapsed = tick.tickInterval;

  const float dt = ticks * static_cast <double> (elapsed);

  auto& cTransform = registry.get <Transform> (entity);

  const auto angle = glm::angleAxis(glm::radians(axis->value * dt),
                                    glm::vec3 {0.0f, 0.0f, 1.0f});

  cTransform.orientation = glm::normalize(angle * cTransform.orientation);

  axis->value = 0.0f;
};

void
loadAudioAssets(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using compos::AudioAssetLoadList;
  using types::AudioAssetManager;

  const auto assetLoadList = std::any_cast <AudioAssetLoadList*> (args.at(0));

  auto& audioManager = registry.ctx().at <AudioAssetManager> ();

  audioManager.load(assetLoadList->audioToLoad);
}

void
loadFontAssets(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using compos::FontAssetLoadList;
  using types::FontAssetManager;

  const auto assetLoadList = std::any_cast <FontAssetLoadList*> (args.at(0));

  auto& audioManager = registry.ctx().at <FontAssetManager> ();

  audioManager.load(assetLoadList->fontsToLoad);
}

void
loadGeometryAssets(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using compos::GeometryAssetLoadList;
  using types::GeometryAssetManager;

  const auto assetLoadList = std::any_cast <GeometryAssetLoadList*> (args.at(0));

  auto& audioManager = registry.ctx().at <GeometryAssetManager> ();

  audioManager.load(assetLoadList->geometryToLoad);
}

void
loadTextStringAssets(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using compos::TextStringAssetLoadList;
  using types::TextStringAssetManager;

  const auto assetLoadList = std::any_cast <TextStringAssetLoadList*> (args.at(0));

  auto& audioManager = registry.ctx().at <TextStringAssetManager> ();

  audioManager.load(assetLoadList->textToLoad);
}

void
loadTextureAssets(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using compos::TextureAssetLoadList;
  using types::TextureAssetManager;

  const auto assetLoadList = std::any_cast <TextureAssetLoadList*> (args.at(0));

  auto& audioManager = registry.ctx().at <TextureAssetManager> ();

  audioManager.load(assetLoadList->texturesToLoad);
}

void
unloadAudioAssets(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using compos::AudioAssetUnloadList;
  using types::AudioAssetManager;

  const auto assetUnloadList = std::any_cast <AudioAssetUnloadList*> (args.at(0));

  auto& audioManager = registry.ctx().at <AudioAssetManager> ();

  audioManager.unload(assetUnloadList->audioToUnload);
}

void
unloadFontAssets(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using compos::FontAssetUnloadList;
  using types::FontAssetManager;

  const auto assetUnloadList = std::any_cast <FontAssetUnloadList*> (args.at(0));

  auto& audioManager = registry.ctx().at <FontAssetManager> ();

  audioManager.unload(assetUnloadList->fontsToUnload);
}

void
unloadGeometryAssets(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using compos::GeometryAssetUnloadList;
  using types::GeometryAssetManager;

  const auto assetUnloadList = std::any_cast <GeometryAssetUnloadList*> (args.at(0));

  auto& audioManager = registry.ctx().at <GeometryAssetManager> ();

  audioManager.unload(assetUnloadList->geometryToUnload);
}

void
unloadTextStringAssets(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using compos::TextStringAssetUnloadList;
  using types::TextStringAssetManager;

  const auto assetUnloadList = std::any_cast <TextStringAssetUnloadList*> (args.at(0));

  auto& audioManager = registry.ctx().at <TextStringAssetManager> ();

  audioManager.unload(assetUnloadList->textToUnload);
}

void
unloadTextureAssets(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using compos::TextureAssetUnloadList;
  using types::TextureAssetManager;

  const auto assetUnloadList = std::any_cast <TextureAssetUnloadList*> (args.at(0));

  auto& audioManager = registry.ctx().at <TextureAssetManager> ();

  audioManager.unload(assetUnloadList->texturesToUnload);
}

} // namespace cqde::callbacks
