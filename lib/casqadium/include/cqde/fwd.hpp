#pragma once


namespace cqde
{

class identifier;

namespace compos
{

struct Camera;
struct EntityMetaInfo;
struct GeometryBuffer;
struct InputController;
struct SceneNode;
struct SequenceController;
struct SnapshotExcluded;
struct SubscriberInput;
struct SubscriberUpdate;
struct Tag;
struct TextureBuffer;
struct Transform;

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
struct TickCurrent;
struct VertexBuffer;

class CallbackManager;
class ConfigManager;
class Package;
class PackageManager;
class SystemManager;

} // namespace types

namespace ui
{

class PackageFilter;
class RegistryFilter;
class StringFilter;
class ToggleButton;

class AssetManagerUi;
class CallbackManagerUi;
class EntityManagerUi;
class InputManagerUi;
class PackageManagerUi;
class SystemManagerUi;
class ViewportManagerUi;

} // namespace ui

} // namespace cqde
