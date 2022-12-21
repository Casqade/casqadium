#pragma once


namespace cqde
{

class identifier;

namespace compos
{

struct Camera;
struct CasqadiumEditorCameraSettings;
struct CasqadiumEditorInternal;
struct CasqadiumEntryPoint;
struct EntityList;
struct EntityMetaInfo;
struct GeometryBuffer;
struct InputController;
struct InteractionListener;
struct InteractionListenerColor;
struct InteractionProbe;
struct InteractionSource;
struct InteractionSourceActiveAction;
struct LightSource;
struct LightTarget;
struct SceneNode;
struct SequenceController;
struct SnapshotExcluded;
struct SubscriberInput;
struct SubscriberUpdate;
struct SystemList;
struct Tag;
struct TextureBuffer;
struct TextureTint;
struct Transform;
struct WantsMouseCentered;
struct WantsMouseHidden;

struct AudioAssetList;
struct FontAssetList;
struct GeometryAssetList;
struct TerrainAssetList;
struct TextStringAssetList;
struct TextureAssetList;

struct CollisionBody;
struct ForceEmitter;
struct RigidBody;

}

namespace types
{

struct EntityReference;
struct FrameCurrent;
struct ControlAxis;
struct InputAxisComparator;
struct InputBinding;
struct InputBindingComparator;
struct InputEvent;
struct System;
struct Terrain;
struct TickCurrent;
struct VertexBuffer;

class Collider;
class ColliderFactory;
class Package;
class SequenceStep;
class SequenceFactory;
class SoloudQueue;
class SplineCurve;

class CallbackManager;
class ConfigManager;
class EntityManager;
class InputManager;
class PackageManager;
class PhysicsManager;
class PrefabManager;
class SnapshotManager;
class SystemManager;
class UserManager;

} // namespace types

namespace ui
{

class PackageFilter;
class RegistryFilter;
class StringFilter;
class ToggleButton;

class AssetManagerUi;
class EntityManagerUi;
class InputManagerUi;
class PackageManagerUi;
class PrefabManagerUi;
class SystemManagerUi;
class ViewportManagerUi;

} // namespace ui

} // namespace cqde
