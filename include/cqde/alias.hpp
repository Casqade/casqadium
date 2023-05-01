#pragma once

#include <cqde/fwd.hpp>
#include <cqde/identifier.hpp>


namespace cqde
{

using ComponentType = entt::id_type;
using EntityType = entt::id_type;

using ActionId = identifier;
using AudioId = identifier;
using AudioGroupId = identifier;
using CallbackId = identifier;
using SystemId = identifier;
using EntityId = identifier;
using FontId = identifier;
using MeshId = identifier;
using MouseCursorId = identifier;
using InputAxisId = identifier;
using InputHwId = identifier;
using PackageId = identifier;
using PrefabId = identifier;
using StringId = identifier;
using TerrainId = identifier;
using TextureId = identifier;
using UserId = identifier;

using InputHwCode = int32_t;
using AudioHandle = uint32_t;

constexpr AudioHandle AudioHandleInvalid{0xfffff000};


} // namespace cqde
