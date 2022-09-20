#include <cqde/callbacks.hpp>

#include <cqde/types/TickCurrent.hpp>

#include <cqde/types/EntityManager.hpp>
#include <cqde/types/SystemManager.hpp>

#include <cqde/types/input/InputManager.hpp>
#include <cqde/types/input/InputBindingRelative.hpp>

#include <cqde/types/ui/EntityManagerUi.hpp>
#include <cqde/types/ui/SystemManagerUi.hpp>
#include <cqde/types/ui/ViewportManagerUi.hpp>

#include <cqde/components/Tag.hpp>
#include <cqde/components/Camera.hpp>
#include <cqde/components/Transform.hpp>
#include <cqde/components/SceneNode.hpp>
#include <cqde/components/EntityMetaInfo.hpp>
#include <cqde/components/SubscriberInput.hpp>
#include <cqde/components/SubscriberUpdate.hpp>
#include <cqde/components/InputController.hpp>
#include <cqde/components/CasqadiumEditorInternal.hpp>

#include <cqde/file_helpers.hpp>
#include <cqde/json_helpers.hpp>
#include <cqde/math_helpers.hpp>

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
  using ui::SystemManagerUi;
  using types::SystemManager;

  using Phase = types::System::Phase;

  editorCameraCreate(registry, args);
  editorControllerCreate(registry, args);

  auto& systemManager = registry.ctx().at <SystemManager> ();

  registry.ctx().at <SystemManagerUi> ().init(registry);

  systemManager.deactivate();

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
  using types::EntityManager;

  for ( const auto&& editorCamera
          : registry.view <Camera, CasqadiumEditorInternal> ().each() )
    return;

  auto& entityManager = registry.ctx().at <EntityManager> ();

  auto eCamera = registry.create();
  auto& cTag = registry.emplace <Tag> (eCamera);

  cTag.id = "cqde_editor_camera";
  entityManager.idRegister(cTag.id, eCamera);

  registry.emplace <SubscriberUpdate> (eCamera);
  registry.emplace <CasqadiumEditorInternal> (eCamera);

  auto& cMetaInfo = registry.emplace <EntityMetaInfo> (eCamera);
  cMetaInfo.packageId = "";

  auto& cCamera = registry.emplace <Camera> (eCamera);
  auto& cTransform = registry.emplace <Transform> (eCamera);
  auto& cSceneNode = registry.emplace <SceneNode> (eCamera);

  cCamera.renderMode = Camera::RenderMode::Wireframe;

  auto& cInputController = registry.emplace <InputController> (eCamera);

  auto& iTranslateX = cInputController.axes["TranslateX"];
  auto& iTranslateY = cInputController.axes["TranslateY"];
  auto& iTranslateZ = cInputController.axes["TranslateZ"];

  iTranslateX.constraint = {1.0f, 0.0f};
  iTranslateY.constraint = {1.0f, 0.0f};
  iTranslateZ.constraint = {1.0f, 0.0f};

  iTranslateX.callbacks.insert("ControlTranslateXRelative");
  iTranslateY.callbacks.insert("ControlTranslateYRelative");
  iTranslateZ.callbacks.insert("ControlTranslateZRelative");

  auto& iPitch = cInputController.axes["Pitch"];
  auto& iYaw = cInputController.axes["Yaw"];
  auto& iRoll = cInputController.axes["Roll"];

  iPitch.constraint = {1.0f, 0.0f};
  iYaw.constraint = {1.0f, 0.0f};
  iRoll.constraint = {1.0f, 0.0f};

  iPitch.callbacks.insert("ControlPitchRelative");
  iYaw.callbacks.insert("ControlYawRelative");
  iRoll.callbacks.insert("ControlRollRelative");

  auto& iCameraControlOff = cInputController.axes["EditorCameraControlOff"];

  iCameraControlOff.callbacks.insert("EntityInputOff");
  iCameraControlOff.callbacks.insert("MouseAutoCenterDisable");
  iCameraControlOff.callbacks.insert("MouseCursorShow");

  auto& iCameraFov = cInputController.axes["EditorCameraFov"];
  iCameraFov.value = cCamera.fov;
  iCameraFov.callbacks.insert("EditorCameraFovControl");
  iCameraFov.constraint = { glm::epsilon <float> (),
                            glm::pi <float> () - glm::epsilon <float> () };

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
  using types::EntityManager;

  for ( const auto&& editorController
          : registry.view <InputController, CasqadiumEditorInternal> (entt::exclude <Camera>).each() )
    return;

  auto& entityManager = registry.ctx().at <EntityManager> ();

  auto eEditorController = registry.create();
  auto& cTag = registry.emplace <Tag> (eEditorController);

  cTag.id = "cqde_editor_controller";
  entityManager.idRegister(cTag.id, eEditorController);

  auto& cMetaInfo = registry.emplace <EntityMetaInfo> (eEditorController);
  cMetaInfo.packageId = "";

  registry.emplace <SubscriberInput> (eEditorController);
  registry.emplace <CasqadiumEditorInternal> (eEditorController);

  auto& cInputController = registry.emplace <InputController> (eEditorController);

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
}

void
editorCameraControlOn(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using types::EntityManager;
  using ui::ViewportManagerUi;
  using compos::Tag;
  using compos::Camera;
  using compos::SubscriberInput;
  using compos::CasqadiumEditorInternal;

  for ( const auto&& [eCamera, cTag, cCamera]
          : registry.view <Tag, Camera, CasqadiumEditorInternal> ().each() )
  {
    if ( registry.ctx().at <ViewportManagerUi> ().mouseOverViewport(cTag.id) == false )
      continue;

    if ( registry.all_of <SubscriberInput> (eCamera) == true )
      continue;

    entityInputOn(registry, {eCamera});

    mouseAutoCenterEnable(registry, args);
    mouseCursorHide(registry, args);
  }
};

void
editorCameraFovControl(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using compos::Camera;
  using compos::InputController;

  const auto eCamera = std::any_cast <entt::entity> (args.at(0));
  const auto cController = std::any_cast <InputController*> (args.at(1));

  auto& cCamera = registry.get <Camera> (eCamera);

  if ( cCamera.projectionType == Camera::Projection::Perspective )
    cCamera.fov = cController->axes["EditorCameraFov"].value;
};

void
editorCameraZoomControl(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using compos::Camera;
  using compos::InputController;

  const auto eCamera = std::any_cast <entt::entity> (args.at(0));
  const auto cController = std::any_cast <InputController*> (args.at(1));

  auto& cCamera = registry.get <Camera> (eCamera);

  if ( cCamera.projectionType == Camera::Projection::Orthographic )
    cCamera.fov = cController->axes["EditorCameraZoom"].value;
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
controlTranslateXRelative(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using types::TickCurrent;
  using compos::Transform;
  using compos::InputController;

  const auto entity = std::any_cast <entt::entity> (args.at(0));
  auto cController = std::any_cast <InputController*> (args.at(1));

  const auto& tick = registry.ctx().at <TickCurrent> ();
  const auto ticks = tick.ticksElapsed;
  const auto elapsed = tick.tickInterval;

  const float dt = ticks * static_cast <double> (elapsed);

  auto& cTransform = registry.get <Transform> (entity);

  auto& axisValue = cController->axes["TranslateX"].value;

  cTransform.translation += cTransform.right() * axisValue * dt;

  axisValue = 0.0f;
};

void
controlTranslateYRelative(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using types::TickCurrent;
  using compos::Transform;
  using compos::InputController;

  const auto entity = std::any_cast <entt::entity> (args.at(0));
  auto cController = std::any_cast <InputController*> (args.at(1));

  const auto& tick = registry.ctx().at <TickCurrent> ();
  const auto ticks = tick.ticksElapsed;
  const auto elapsed = tick.tickInterval;

  const float dt = ticks * static_cast <double> (elapsed);

  auto& cTransform = registry.get <Transform> (entity);

  auto& axisValue = cController->axes["TranslateY"].value;

  cTransform.translation += cTransform.up() * axisValue * dt;

  axisValue = 0.0f;
};

void
controlTranslateZRelative(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using types::TickCurrent;
  using compos::Transform;
  using compos::InputController;

  const auto entity = std::any_cast <entt::entity> (args.at(0));
  auto cController = std::any_cast <InputController*> (args.at(1));

  const auto& tick = registry.ctx().at <TickCurrent> ();
  const auto ticks = tick.ticksElapsed;
  const auto elapsed = tick.tickInterval;

  const float dt = ticks * static_cast <double> (elapsed);

  auto& cTransform = registry.get <Transform> (entity);

  auto& axisValue = cController->axes["TranslateZ"].value;

  cTransform.translation += cTransform.front() * axisValue * dt;

  axisValue = 0.0f;
};

void
controlPitchRelative(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using compos::Transform;
  using compos::InputController;

  const auto entity = std::any_cast <entt::entity> (args.at(0));
  auto cController = std::any_cast <InputController*> (args.at(1));

  auto& cTransform = registry.get <Transform> (entity);

  auto& axisValue = cController->axes["Pitch"].value;

  const auto angle = glm::angleAxis(glm::radians(axisValue),
                                    glm::vec3 {1.0f, 0.0f, 0.0f});

  const auto orientationPrev = cTransform.orientation;

  cTransform.orientation = glm::normalize(cTransform.orientation * angle);

  if ( cTransform.up().y < 0.0f )
    cTransform.orientation = orientationPrev;

  axisValue = 0.0f;
};

void
controlYawRelative(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using compos::Transform;
  using compos::InputController;

  const auto entity = std::any_cast <entt::entity> (args.at(0));
  auto cController = std::any_cast <InputController*> (args.at(1));

  auto& cTransform = registry.get <Transform> (entity);

  auto& axisValue = cController->axes["Yaw"].value;

  const auto angle = glm::angleAxis(glm::radians(axisValue),
                                    glm::vec3 {0.0f, 1.0f, 0.0f});

  cTransform.orientation = glm::normalize(angle * cTransform.orientation);

  axisValue = 0.0f;
};

void
controlRollRelative(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using compos::Transform;
  using compos::InputController;

  const auto entity = std::any_cast <entt::entity> (args.at(0));
  auto cController = std::any_cast <InputController*> (args.at(1));

  auto& cTransform = registry.get <Transform> (entity);

  auto& axisValue = cController->axes["Roll"].value;

  const auto angle = glm::angleAxis(glm::radians(axisValue),
                                    glm::vec3 {0.0f, 0.0f, 1.0f});

  cTransform.orientation = glm::normalize(angle * cTransform.orientation);

  axisValue = 0.0f;
};

} // namespace cqde::callbacks
