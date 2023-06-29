#include <cqde/types/CasqadiumEngine.hpp>

#include <cqde/types/EntityManager.hpp>

#include <cqde/components/Camera.hpp>
#include <cqde/components/CasqadiumEntryPoint.hpp>
#include <cqde/components/CasqadiumEditorInternal.hpp>
#include <cqde/components/CasqadiumEditorCameraSettings.hpp>
#include <cqde/components/EntityMetaInfo.hpp>
#include <cqde/components/DrawableMesh.hpp>
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
#include <cqde/components/SkinPool.hpp>
#include <cqde/components/Tag.hpp>
#include <cqde/components/TextureTint.hpp>
#include <cqde/components/Transform.hpp>
#include <cqde/components/WantsMouseGrabbed.hpp>
#include <cqde/components/WantsCursorOverridden.hpp>

#include <cqde/components/EntityList.hpp>
#include <cqde/components/SystemList.hpp>

#include <cqde/components/assets/AudioAssetList.hpp>
#include <cqde/components/assets/FontAssetList.hpp>
#include <cqde/components/assets/MeshAssetList.hpp>
#include <cqde/components/assets/MouseCursorAssetList.hpp>
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

#include <entt/entity/registry.hpp>
#include <entt/locator/locator.hpp>
#include <entt/meta/context.hpp>


namespace cqde::types
{

void
CasqadiumEngine::registerComponents()
{
  using namespace cqde::compos;

  auto& entityManager = mRegistry.ctx().get <EntityManager> ();

  entityManager.registerComponent <Camera> ("Camera");
  entityManager.registerComponent <CasqadiumEditorCameraSettings> ("CasqadiumEditorCameraSettings");
  entityManager.registerComponent <EntityMetaInfo> ("EntityMetaInfo");
  entityManager.registerComponent <DrawableMesh> ("DrawableMesh");
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
  entityManager.registerComponent <SkinPool> ("SkinPool");
  entityManager.registerComponent <Tag> ("Tag");
  entityManager.registerComponent <TextureTint> ("TextureTint");
  entityManager.registerComponent <Transform> ("Transform");
  entityManager.registerComponent <WantsCursorOverridden> ("WantsCursorOverridden");

  entityManager.registerComponent <EntityList> ("EntityList");
  entityManager.registerComponent <SystemList> ("SystemList");

  entityManager.registerComponent <AudioAssetList> ("AudioAssetList");
  entityManager.registerComponent <FontAssetList> ("FontAssetList");
  entityManager.registerComponent <MeshAssetList> ("MeshAssetList");
  entityManager.registerComponent <MouseCursorAssetList> ("MouseCursorAssetList");
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
  entityManager.registerEmptyComponent <WantsMouseGrabbed> ("WantsMouseGrabbed");

  using MetaCtxLocator = entt::locator <entt::meta_ctx>;
  using MetaCtxHandle = MetaCtxLocator::node_type;

  const auto handle = MetaCtxLocator::handle();
  mRegistry.ctx().emplace <MetaCtxHandle> (handle);
}

} // namespace cqde::types
