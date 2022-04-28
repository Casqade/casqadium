#pragma once

#include <cqde/common.hpp>

#include <entt/core/hashed_string.hpp>

#include <olcPGE/olcPixelGameEngine.hpp>

#include <map>
#include <memory>


namespace cqde
{

typedef entt::hashed_string CallbackId;
typedef entt::hashed_string EntityId;
typedef entt::hashed_string InputSourceId;
typedef entt::hashed_string InputDestinationId;
typedef entt::hashed_string InputCallbackId;
typedef entt::hashed_string PackageId;
typedef entt::hashed_string StringId;
typedef entt::hashed_string TextureId;

namespace types
{

typedef std::map <StringId, std::string,
                  entt::hashed_string_comparator> StringStorage;

typedef std::map <TextureId, std::shared_ptr <olc::Renderable>,
                  entt::hashed_string_comparator> TextureStorage;

typedef std::map <int32_t, InputSourceId> HwControlMap;

typedef std::map <InputSourceId, InputDestinationId,
                  entt::hashed_string_comparator> SwControlMap;

}

} // namespace cqde
