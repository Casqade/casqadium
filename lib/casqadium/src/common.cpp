#include <cqde/common.hpp>

#include <cqde/systems.hpp>
#include <cqde/callbacks.hpp>
#include <cqde/ecs_helpers.hpp>

#include <cqde/components/Camera.hpp>
#include <cqde/components/CasqadiumEntryPoint.hpp>
#include <cqde/components/CasqadiumEditorInternal.hpp>
#include <cqde/components/EntityMetaInfo.hpp>
#include <cqde/components/GeometryBuffer.hpp>
#include <cqde/components/InputController.hpp>
#include <cqde/components/SubscriberInput.hpp>
#include <cqde/components/SceneNode.hpp>
#include <cqde/components/SnapshotExcluded.hpp>
#include <cqde/components/SubscriberUpdate.hpp>
#include <cqde/components/SequenceController.hpp>
#include <cqde/components/Tag.hpp>
#include <cqde/components/TextureBuffer.hpp>
#include <cqde/components/Transform.hpp>
#include <cqde/components/WantsMouseCentered.hpp>
#include <cqde/components/WantsMouseHidden.hpp>

#include <cqde/types/CallbackManager.hpp>
#include <cqde/types/EntityManager.hpp>
#include <cqde/types/input/InputManager.hpp>
#include <cqde/types/PackageManager.hpp>
#include <cqde/types/PrefabManager.hpp>
#include <cqde/types/SystemManager.hpp>
#include <cqde/types/SnapshotManager.hpp>
#include <cqde/types/UserManager.hpp>


#include <cqde/types/TickCurrent.hpp>
#include <cqde/types/FrameCurrent.hpp>

#include <cqde/types/assets/AudioAssetManager.hpp>
#include <cqde/types/assets/FontAssetManager.hpp>
#include <cqde/types/assets/GeometryAssetManager.hpp>
#include <cqde/types/assets/TextureAssetManager.hpp>
#include <cqde/types/assets/TextStringAssetManager.hpp>

#include <cqde/types/ui/AssetManagerUi.hpp>
#include <cqde/types/ui/CallbackManagerUi.hpp>
#include <cqde/types/ui/EntityManagerUi.hpp>
#include <cqde/types/ui/InputManagerUi.hpp>
#include <cqde/types/ui/PackageManagerUi.hpp>
#include <cqde/types/ui/PrefabManagerUi.hpp>
#include <cqde/types/ui/SystemManagerUi.hpp>
#include <cqde/types/ui/ViewportManagerUi.hpp>

#include <entt/entity/registry.hpp>

#include <thirdparty/ctpl/ctpl_stl.h>


namespace cqde
{

void
engineInit( entt::registry& registry )
{
  using namespace callbacks;
  using namespace systems;
  using namespace compos;
  using namespace types;
  using namespace ui;

//  entt::meta_ctx::bind(registry.ctx().emplace <entt::meta_ctx> ());

  auto& audioBackend = registry.ctx().emplace <SoLoud::Soloud> ();

  registry.ctx().emplace <TickCurrent> ();
  registry.ctx().emplace <FrameCurrent> ();

  auto& callbackManager = registry.ctx().emplace <CallbackManager> ();
  auto& entityManager = registry.ctx().emplace <EntityManager> ();
  auto& inputManager = registry.ctx().emplace <InputManager> ();
  auto& packageManager = registry.ctx().emplace <PackageManager> ();
  auto& prefabManager = registry.ctx().emplace <PrefabManager> ();
  auto& systemManager = registry.ctx().emplace <SystemManager> ();
  auto& snapshotManager = registry.ctx().emplace <SnapshotManager> ();
  auto& userManager = registry.ctx().emplace <UserManager> ();

  registry.ctx().emplace <AssetManagerUi> ();
  registry.ctx().emplace <CallbackManagerUi> (&callbackManager);
  registry.ctx().emplace <EntityManagerUi> (&entityManager);
  registry.ctx().emplace <InputManagerUi> (&inputManager);
  registry.ctx().emplace <PackageManagerUi> (&packageManager);
  registry.ctx().emplace <PrefabManagerUi> (&prefabManager);
  registry.ctx().emplace <SystemManagerUi> (&systemManager);
  registry.ctx().emplace <ViewportManagerUi> ();

  auto& tp = registry.ctx().emplace <ctpl::thread_pool> (std::thread::hardware_concurrency() | 1);

  registry.ctx().emplace <AudioAssetManager> (tp);
  registry.ctx().emplace <FontAssetManager> (tp);
  registry.ctx().emplace <GeometryAssetManager> (tp);
  registry.ctx().emplace <TextureAssetManager> (tp);
  registry.ctx().emplace <TextStringAssetManager> (tp);

  entityManager.registerComponent <Camera> ("Camera");
  entityManager.registerComponent <CasqadiumEntryPoint> ("CasqadiumEntryPoint");
  entityManager.registerComponent <EntityMetaInfo> ("EntityMetaInfo");
  entityManager.registerComponent <GeometryBuffer> ("GeometryBuffer");
  entityManager.registerComponent <InputController> ("InputController");
  entityManager.registerComponent <SceneNode> ("SceneNode");
  entityManager.registerComponent <SequenceController> ("SequenceController");
  entityManager.registerComponent <Tag> ("Tag");
  entityManager.registerComponent <TextureBuffer> ("TextureBuffer");
  entityManager.registerComponent <Transform> ("Transform");

//  Components-tags
  entityManager.registerEmptyComponent <CasqadiumEditorInternal> ("CasqadiumEditorInternal");
  entityManager.registerEmptyComponent <SnapshotExcluded> ("SnapshotExcluded");
  entityManager.registerEmptyComponent <SubscriberInput> ("SubscriberInput");
  entityManager.registerEmptyComponent <SubscriberUpdate> ("SubscriberUpdate");
  entityManager.registerEmptyComponent <WantsMouseCentered> ("WantsMouseCentered");
  entityManager.registerEmptyComponent <WantsMouseHidden> ("WantsMouseHidden");


//  Callbacks
  callbackManager.Register("MouseAutoCenterEnable", mouseAutoCenterEnable);
  callbackManager.Register("MouseAutoCenterDisable", mouseAutoCenterDisable);
  callbackManager.Register("MouseAutoCenterToggle", mouseAutoCenterToggle);

  callbackManager.Register("MouseCursorHide", mouseCursorHide);
  callbackManager.Register("MouseCursorShow", mouseCursorShow);
  callbackManager.Register("MouseGrabToggle", mouseGrabToggle);


  callbackManager.Register("EditorModeEnable", editorModeEnable);
  callbackManager.Register("EditorModeDisable", editorModeDisable);
  callbackManager.Register("EditorModeToggle", editorModeToggle);

  callbackManager.Register("EditorGizmoSetModeTranslate", editorGizmoSetModeTranslate);
  callbackManager.Register("EditorGizmoSetModeRotate", editorGizmoSetModeRotate);
  callbackManager.Register("EditorGizmoSetModeScale", editorGizmoSetModeScale);

  callbackManager.Register("EditorGizmoSetSpaceLocal", editorGizmoSetSpaceLocal);
  callbackManager.Register("EditorGizmoSetSpaceWorld", editorGizmoSetSpaceWorld);

  callbackManager.Register("EditorEntityMultipleSelectionEnable", editorEntityMultipleSelectionEnable);
  callbackManager.Register("EditorEntityMultipleSelectionDisable", editorEntityMultipleSelectionDisable);
  callbackManager.Register("EditorEntityMultipleSelectionToggle", editorEntityMultipleSelectionToggle);

  callbackManager.Register("EditorCameraCreate", editorCameraCreate);
  callbackManager.Register("EditorControllerCreate", editorControllerCreate);
  callbackManager.Register("EditorBindingsAssign", editorBindingsAssign);

  callbackManager.Register("EditorCameraControlOn", editorCameraControlOn);
  callbackManager.Register("EditorCameraControlOff", editorCameraControlOff);
  callbackManager.Register("EditorCameraFovControl", editorCameraFovControl);
  callbackManager.Register("EditorCameraZoomControl", editorCameraZoomControl);
  callbackManager.Register("EditorEntitySelect", editorEntitySelect);


  callbackManager.Register("QuickSave", quickSave);
  callbackManager.Register("QuickLoad", quickLoad);


  callbackManager.Register("EntityInputOn", entityInputOn);
  callbackManager.Register("EntityInputOff", entityInputOff);
  callbackManager.Register("EntityInputToggle", entityInputToggle);

  callbackManager.Register("EntityUpdateOn", entityUpdateOn);
  callbackManager.Register("EntityUpdateOff", entityUpdateOff);
  callbackManager.Register("EntityUpdateToggle", entityUpdateToggle);

  callbackManager.Register("ControlTranslateXRelative", controlTranslateXRelative);
  callbackManager.Register("ControlTranslateYRelative", controlTranslateYRelative);
  callbackManager.Register("ControlTranslateZRelative", controlTranslateZRelative);

  callbackManager.Register("ControlPitchRelative", controlPitchRelative);
  callbackManager.Register("ControlYawRelative", controlYawRelative);
  callbackManager.Register("ControlRollRelative", controlRollRelative);

  using Phase = System::Phase;

  systemManager.Register("CasqadiumEditorSystem",
                         EditorSystem,
                         Phase{Phase::Logic | Phase::Editor});

  systemManager.Register("CasqadiumEditorMouseCenteringSystem",
                         MouseCenteringSystem,
                         Phase{Phase::Logic | Phase::Editor});

  systemManager.Register("CasqadiumEditorMouseHidingSystem",
                         MouseHidingSystem,
                         Phase{Phase::Logic | Phase::Editor});

  systemManager.Register("MouseCenteringSystem",
                         MouseCenteringSystem,
                         Phase::Logic);

  systemManager.Register("MouseHidingSystem",
                         MouseHidingSystem,
                         Phase::Logic);

  systemManager.Register("CullingSystem",
                         CullingSystem,
                         Phase::Render);

  systemManager.Register("RenderSystem",
                         RenderSystem,
                         Phase::Render);

  systemManager.Register("CasqadiumEditorRenderSystem",
                         RenderSystem,
                         Phase{Phase::Render | Phase::Editor});

  systemManager.Register("CasqadiumEditorEntityHighlightSystem",
                         EditorEntityHighlightSystem,
                         Phase{Phase::Render | Phase::Editor});
}

std::string
toLowerCase(
  const std::string& str )
{
  std::string result = str;

  for ( auto& c : result )
    c = std::tolower(c);

  return result;
}

std::vector <std::string>
stringSplit(
  const std::string& string,
  const char delimeter )
{
  std::vector <std::string> result {};

  std::stringstream stream {string};
  std::string token {};

  while ( getline (stream, token, delimeter) )
    result.push_back(token);

  return result;
}

std::string stringJoin(
  const std::vector <std::string>& strings,
  const std::string& delimeter )
{
  std::string result {};

  for ( auto iter = strings.begin();
        iter != strings.end();
        ++iter )
    if ( iter == strings.end() - 1 )
      result += *iter;
    else
      result += *iter + delimeter;

  return result;
}

} // namespace cqde
