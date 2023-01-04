#include <cqde/common.hpp>

#include <cqde/systems.hpp>
#include <cqde/callbacks.hpp>
#include <cqde/ecs_helpers.hpp>

#include <cqde/components/Camera.hpp>
#include <cqde/components/CasqadiumEntryPoint.hpp>
#include <cqde/components/CasqadiumEditorInternal.hpp>
#include <cqde/components/CasqadiumEditorCameraSettings.hpp>
#include <cqde/components/EntityMetaInfo.hpp>
#include <cqde/components/GeometryBuffer.hpp>
#include <cqde/components/InputController.hpp>
#include <cqde/components/InteractionListener.hpp>
#include <cqde/components/InteractionListenerColor.hpp>
#include <cqde/components/InteractionProbe.hpp>
#include <cqde/components/InteractionSource.hpp>
#include <cqde/components/InteractionSourceActiveAction.hpp>
#include <cqde/components/LightSource.hpp>
#include <cqde/components/LightTarget.hpp>
#include <cqde/components/SubscriberInput.hpp>
#include <cqde/components/SceneNode.hpp>
#include <cqde/components/SnapshotExcluded.hpp>
#include <cqde/components/SubscriberUpdate.hpp>
#include <cqde/components/SequenceController.hpp>
#include <cqde/components/Tag.hpp>
#include <cqde/components/TextureBuffer.hpp>
#include <cqde/components/TextureTint.hpp>
#include <cqde/components/Transform.hpp>
#include <cqde/components/WantsMouseCentered.hpp>
#include <cqde/components/WantsMouseHidden.hpp>

#include <cqde/components/EntityList.hpp>
#include <cqde/components/SystemList.hpp>

#include <cqde/components/assets/AudioAssetList.hpp>
#include <cqde/components/assets/FontAssetList.hpp>
#include <cqde/components/assets/GeometryAssetList.hpp>
#include <cqde/components/assets/TerrainAssetList.hpp>
#include <cqde/components/assets/TextStringAssetList.hpp>
#include <cqde/components/assets/TextureAssetList.hpp>

#include <cqde/components/audio/Audio3dParams.hpp>
#include <cqde/components/audio/AudioBus.hpp>
#include <cqde/components/audio/AudioDrivenTransform.hpp>
#include <cqde/components/audio/AudioGroupMap.hpp>
#include <cqde/components/audio/AudioListener3d.hpp>
#include <cqde/components/audio/AudioLoop.hpp>
#include <cqde/components/audio/AudioSequence.hpp>

#include <cqde/components/physics/CollisionBody.hpp>
#include <cqde/components/physics/RigidBody.hpp>
#include <cqde/components/physics/ForceEmitter.hpp>

#include <cqde/types/CallbackManager.hpp>
#include <cqde/types/EntityManager.hpp>
#include <cqde/types/input/InputManager.hpp>
#include <cqde/types/PackageManager.hpp>
#include <cqde/types/PhysicsManager.hpp>
#include <cqde/types/PrefabManager.hpp>
#include <cqde/types/SystemManager.hpp>
#include <cqde/types/SnapshotManager.hpp>
#include <cqde/types/UserManager.hpp>

#include <cqde/types/physics/ColliderFactory.hpp>
#include <cqde/types/physics/colliders/ColliderBox.hpp>
#include <cqde/types/physics/colliders/ColliderCapsule.hpp>
#include <cqde/types/physics/colliders/ColliderHeightField.hpp>
#include <cqde/types/physics/colliders/ColliderSphere.hpp>

#include <cqde/types/sequences/SequenceFactory.hpp>
#include <cqde/types/sequences/CallbackExecute.hpp>
#include <cqde/types/sequences/CallbackExecuteAPeriodic.hpp>
#include <cqde/types/sequences/CallbackExecutePeriodic.hpp>
#include <cqde/types/sequences/CameraFovInterpolated.hpp>
#include <cqde/types/sequences/Delay.hpp>
#include <cqde/types/sequences/RandomDelay.hpp>
#include <cqde/types/sequences/TextureTintInterpolated.hpp>
#include <cqde/types/sequences/TransformInterpolated.hpp>
#include <cqde/types/sequences/TransformManipulate.hpp>

#include <cqde/types/TickCurrent.hpp>
#include <cqde/types/FrameCurrent.hpp>

#include <cqde/types/audio/AudioFilterFactory.hpp>

#include <cqde/types/assets/AudioAssetManager.hpp>
#include <cqde/types/assets/FontAssetManager.hpp>
#include <cqde/types/assets/GeometryAssetManager.hpp>
#include <cqde/types/assets/TerrainAssetManager.hpp>
#include <cqde/types/assets/TextureAssetManager.hpp>
#include <cqde/types/assets/TextStringAssetManager.hpp>

#include <entt/entity/registry.hpp>
#include <entt/meta/context.hpp>

#include <ctpl/ctpl_stl.h>

#include <soloud_bassboostfilter.h>
#include <soloud_biquadresonantfilter.h>
#include <soloud_duckfilter.h>
#include <soloud_echofilter.h>
#include <soloud_eqfilter.h>
#include <soloud_flangerfilter.h>
#include <soloud_freeverbfilter.h>
#include <soloud_lofifilter.h>
#include <soloud_robotizefilter.h>
#include <soloud_waveshaperfilter.h>

#include <locale>
#include <codecvt>


namespace cqde
{

void
engineInit(
  entt::registry& registry )
{
  using namespace callbacks;
  using namespace systems;
  using namespace compos;
  using namespace types;
  using namespace ui;

  registry.ctx().emplace <SoLoud::Soloud> ();

  registry.ctx().emplace <TickCurrent> ();
  registry.ctx().emplace <FrameCurrent> ();

  auto& callbackManager = registry.ctx().emplace <CallbackManager> ();
  auto& entityManager = registry.ctx().emplace <EntityManager> ();
  auto& inputManager = registry.ctx().emplace <InputManager> ();
  auto& packageManager = registry.ctx().emplace <PackageManager> ();
  auto& physicsManager = registry.ctx().emplace <PhysicsManager> (registry);
  auto& prefabManager = registry.ctx().emplace <PrefabManager> ();
  auto& systemManager = registry.ctx().emplace <SystemManager> ();
  auto& snapshotManager = registry.ctx().emplace <SnapshotManager> ();
  auto& userManager = registry.ctx().emplace <UserManager> ();

  auto& audioFilterFactory = registry.ctx().emplace <AudioFilterFactory> ();
  auto& colliderShapeFactory = registry.ctx().emplace <ColliderFactory> ();
  auto& sequenceFactory = registry.ctx().emplace <SequenceFactory> ();

  auto& tp = registry.ctx().emplace <ctpl::thread_pool> (std::thread::hardware_concurrency() | 1);

  registry.ctx().emplace <AudioAssetManager> (tp);
  registry.ctx().emplace <FontAssetManager> (tp);
  registry.ctx().emplace <GeometryAssetManager> (tp);
  registry.ctx().emplace <TerrainAssetManager> (tp);
  registry.ctx().emplace <TextureAssetManager> (tp);
  registry.ctx().emplace <TextStringAssetManager> (tp);

  entityManager.registerComponent <Camera> ("Camera");
  entityManager.registerComponent <CasqadiumEditorCameraSettings> ("CasqadiumEditorCameraSettings");
  entityManager.registerComponent <EntityMetaInfo> ("EntityMetaInfo");
  entityManager.registerComponent <GeometryBuffer> ("GeometryBuffer");
  entityManager.registerComponent <InputController> ("InputController");
  entityManager.registerComponent <InteractionListener> ("InteractionListener");
  entityManager.registerComponent <InteractionListenerColor> ("InteractionListenerColor");
  entityManager.registerComponent <InteractionProbe> ("InteractionProbe");
  entityManager.registerComponent <InteractionSource> ("InteractionSource");
  entityManager.registerComponent <InteractionSourceActiveAction> ("InteractionSourceActiveAction");
  entityManager.registerComponent <LightSource> ("LightSource");
//  entityManager.registerComponent <LightTarget> ("LightTarget"); // todo: add material data
  entityManager.registerComponent <SceneNode> ("SceneNode");
  entityManager.registerComponent <SequenceController> ("SequenceController");
  entityManager.registerComponent <Tag> ("Tag");
  entityManager.registerComponent <TextureBuffer> ("TextureBuffer");
  entityManager.registerComponent <TextureTint> ("TextureTint");
  entityManager.registerComponent <Transform> ("Transform");

  entityManager.registerComponent <EntityList> ("EntityList");
  entityManager.registerComponent <SystemList> ("SystemList");

  entityManager.registerComponent <AudioAssetList> ("AudioAssetList");
  entityManager.registerComponent <FontAssetList> ("FontAssetList");
  entityManager.registerComponent <GeometryAssetList> ("GeometryAssetList");
  entityManager.registerComponent <TerrainAssetList> ("TerrainAssetList");
  entityManager.registerComponent <TextStringAssetList> ("TextStringAssetList");
  entityManager.registerComponent <TextureAssetList> ("TextureAssetList");

  entityManager.registerComponent <CollisionBody> ("CollisionBody");
  entityManager.registerComponent <RigidBody> ("RigidBody");
  entityManager.registerComponent <ForceEmitter> ("ForceEmitter");

  entityManager.registerEmptyComponent <AudioBus> ("AudioBus");
  entityManager.registerEmptyComponent <AudioListener3d> ("AudioListener3d");
  entityManager.registerEmptyComponent <AudioSequence> ("AudioSequence");

  entityManager.registerComponent <Audio3dParams> ("Audio3dParams");
  entityManager.registerComponent <AudioDrivenTransform> ("AudioDrivenTransform");
  entityManager.registerComponent <AudioGroupMap> ("AudioGroupMap");
  entityManager.registerComponent <AudioLoop> ("AudioLoop");

//  Components-tags
  entityManager.registerEmptyComponent <CasqadiumEntryPoint> ("CasqadiumEntryPoint");
  entityManager.registerEmptyComponent <CasqadiumEditorInternal> ("CasqadiumEditorInternal");
  entityManager.registerEmptyComponent <LightTarget> ("LightTarget"); // todo: make component non-empty
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

  callbackManager.Register("EditorCameraSpeedControl", editorCameraSpeedControl);

  callbackManager.Register("EditorCameraTranslateXRelative", editorCameraTranslateXRelative);
  callbackManager.Register("EditorCameraTranslateYRelative", editorCameraTranslateYRelative);
  callbackManager.Register("EditorCameraTranslateZRelative", editorCameraTranslateZRelative);

  callbackManager.Register("EditorCameraControlOn", editorCameraControlOn);
  callbackManager.Register("EditorCameraControlOff", editorCameraControlOff);
  callbackManager.Register("EditorCameraFovControl", editorCameraFovControl);
  callbackManager.Register("EditorCameraZoomControl", editorCameraZoomControl);
  callbackManager.Register("EditorEntitySelect", editorEntitySelect);

  callbackManager.Register("QuickSave", quickSave);
  callbackManager.Register("QuickLoad", quickLoad);

  callbackManager.Register("Interact", interact);
  callbackManager.Register("InteractProbe", interactProbe);

  callbackManager.Register("EntityRemove", entityRemove);

  callbackManager.Register("EntityUpdateOn", entityUpdateOn);
  callbackManager.Register("EntityUpdateOff", entityUpdateOff);
  callbackManager.Register("EntityUpdateToggle", entityUpdateToggle);
  callbackManager.Register("EntitiesUpdateOn", entitiesUpdateOn);
  callbackManager.Register("EntitiesUpdateOff", entitiesUpdateOff);
  callbackManager.Register("EntitiesUpdateToggle", entitiesUpdateToggle);

  callbackManager.Register("EntityInputOn", entityInputOn);
  callbackManager.Register("EntityInputOff", entityInputOff);
  callbackManager.Register("EntityInputToggle", entityInputToggle);
  callbackManager.Register("EntitiesInputOn", entitiesInputOn);
  callbackManager.Register("EntitiesInputOff", entitiesInputOff);
  callbackManager.Register("EntitiesInputToggle", entitiesInputToggle);

  callbackManager.Register("SystemsActivate", systemsActivate);
  callbackManager.Register("SystemsDeactivate", systemsDeactivate);
  callbackManager.Register("SystemsToggle", systemsToggle);

  callbackManager.Register("LoadAudioAssets", loadAudioAssets);
  callbackManager.Register("LoadFontAssets", loadFontAssets);
  callbackManager.Register("LoadGeometryAssets", loadGeometryAssets);
  callbackManager.Register("LoadTerrainAssets", loadTerrainAssets);
  callbackManager.Register("LoadTextStringAssets", loadTextStringAssets);
  callbackManager.Register("LoadTextureAssets", loadTextureAssets);

  callbackManager.Register("UnloadAudioAssets", unloadAudioAssets);
  callbackManager.Register("UnloadFontAssets", unloadFontAssets);
  callbackManager.Register("UnloadGeometryAssets", unloadGeometryAssets);
  callbackManager.Register("UnloadTerrainAssets", unloadTerrainAssets);
  callbackManager.Register("UnloadTextStringAssets", unloadTextStringAssets);
  callbackManager.Register("UnloadTextureAssets", unloadTextureAssets);

  callbackManager.Register("ControlTranslateXRelative", controlTranslateXRelative);
  callbackManager.Register("ControlTranslateYRelative", controlTranslateYRelative);
  callbackManager.Register("ControlTranslateZRelative", controlTranslateZRelative);

  callbackManager.Register("ControlPitchRelative", controlPitchRelative);
  callbackManager.Register("ControlYawRelative", controlYawRelative);
  callbackManager.Register("ControlRollRelative", controlRollRelative);

  callbackManager.Register("ForceEmitterCallback", forceEmitterCallback);
  callbackManager.Register("GravityEmitterCallback", gravityEmitterCallback);

  callbackManager.Register("ForceXRelative", forceXRelative);
  callbackManager.Register("ForceYRelative", forceYRelative);
  callbackManager.Register("ForceZRelative", forceZRelative);

  callbackManager.Register("TorqueXRelative", torqueXRelative);
  callbackManager.Register("TorqueYRelative", torqueYRelative);
  callbackManager.Register("TorqueZRelative", torqueZRelative);


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

  systemManager.Register("InteractionResetSystem",
                         InteractionResetSystem,
                         Phase::Logic);

  systemManager.Register("PhysicsSystem",
                         PhysicsSystem,
                         Phase::Logic);

  systemManager.Register("SequenceSystem",
                         SequenceSystem,
                         Phase::Logic);

  systemManager.Register("Audio3dSystem",
                         Audio3dSystem,
                         Phase::Logic);

  systemManager.Register("AudioDrivenTransformSystem",
                         AudioDrivenTransformSystem,
                         Phase::Logic);

  systemManager.Register("InteractionQuerySystem",
                         InteractionQuerySystem,
                         Phase::Logic);

  systemManager.Register("MouseCenteringSystem",
                         MouseCenteringSystem,
                         Phase::Logic);

  systemManager.Register("MouseHidingSystem",
                         MouseHidingSystem,
                         Phase::Logic);


  systemManager.Register("RenderBufferClearSystem",
                         RenderBufferClearSystem,
                         Phase::Render);

  systemManager.Register("CasqadiumEditorRenderBufferClearSystem",
                         RenderBufferClearSystem,
                         Phase{Phase::Render | Phase::Editor});

  systemManager.Register("CullingSystem",
                         CullingSystem,
                         Phase::Render);

  systemManager.Register("CasqadiumEditorCullingSystem",
                         EditorCullingSystem,
                         Phase{Phase::Render | Phase::Editor});

  systemManager.Register("LightingSystem",
                         LightingSystem,
                         Phase::Render);

  systemManager.Register("RenderSystem",
                         RenderSystem,
                         Phase::Render);

  systemManager.Register("PhysicsDebugRenderSystem",
                         PhysicsDebugRenderSystem,
                         Phase::Render);


  systemManager.Register("CasqadiumEditorRenderSystem",
                         RenderSystem,
                         Phase{Phase::Render | Phase::Editor});

  systemManager.Register("CasqadiumEditorEntityHighlightSystem",
                         EditorEntityHighlightSystem,
                         Phase{Phase::Render | Phase::Editor});

  systemManager.Register("EditorPhysicsDebugRenderSystem",
                         EditorPhysicsDebugRenderSystem,
                         Phase::Render);

  systemManager.Register("InteractionHighlightSystem",
                         InteractionHighlightSystem,
                         Phase::Render);


  audioFilterFactory.registerFilter <SoLoud::BassboostFilter> ("BassboostFilter");
  audioFilterFactory.registerFilter <SoLoud::BiquadResonantFilter> ("BiquadResonantFilter");
  audioFilterFactory.registerFilter <SoLoud::DuckFilter> ("DuckFilter");
  audioFilterFactory.registerFilter <SoLoud::EchoFilter> ("EchoFilter");
  audioFilterFactory.registerFilter <SoLoud::EqFilter> ("EqFilter");
  audioFilterFactory.registerFilter <SoLoud::FlangerFilter> ("FlangerFilter");
  audioFilterFactory.registerFilter <SoLoud::FreeverbFilter> ("FreeverbFilter");
  audioFilterFactory.registerFilter <SoLoud::LofiFilter> ("LofiFilter");
  audioFilterFactory.registerFilter <SoLoud::RobotizeFilter> ("RobotizeFilter");
  audioFilterFactory.registerFilter <SoLoud::WaveShaperFilter> ("WaveShaperFilter");


  colliderShapeFactory.registerCollider <ColliderBox> ("BoxShape");
  colliderShapeFactory.registerCollider <ColliderCapsule> ("CapsuleShape");
  colliderShapeFactory.registerCollider <ColliderHeightField> ("HeightFieldShape");
  colliderShapeFactory.registerCollider <ColliderSphere> ("SphereShape");


  sequenceFactory.registerSequence <CameraFovInterpolated> ("CameraFovInterpolated");
  sequenceFactory.registerSequence <Delay> ("Delay");
  sequenceFactory.registerSequence <RandomDelay> ("RandomDelay");
  sequenceFactory.registerSequence <CallbackExecute> ("CallbackExecute");
  sequenceFactory.registerSequence <CallbackExecuteAPeriodic> ("CallbackExecuteAPeriodic");
  sequenceFactory.registerSequence <CallbackExecutePeriodic> ("CallbackExecutePeriodic");
  sequenceFactory.registerSequence <TextureTintInterpolated> ("TextureTintInterpolated");
  sequenceFactory.registerSequence <TransformInterpolated> ("TransformInterpolated");
  sequenceFactory.registerSequence <TransformManipulate> ("TransformManipulate");


  using MetaCtxLocator = entt::locator <entt::meta_ctx>;
  using MetaCtxHandle = MetaCtxLocator::node_type;

  const auto handle = MetaCtxLocator::handle();
  registry.ctx().emplace <MetaCtxHandle> (handle);
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

std::string
stringJoin(
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

std::string
stringToUTF8(
  const std::u32string& source )
{
  return std::wstring_convert <std::codecvt_utf8_utf16 <char32_t>, char32_t> {}.to_bytes(source);
}

std::u32string
stringFromUTF8(
  const std::string& source )
{
  return std::wstring_convert <std::codecvt_utf8_utf16 <char32_t>, char32_t> {}.from_bytes(source);
}

} // namespace cqde
