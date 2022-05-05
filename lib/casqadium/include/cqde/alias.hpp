#pragma once

#include <cqde/fwd.hpp>
#include <cqde/identifier.hpp>

#include <olcPGE/olcPixelGameEngine.hpp>

#include <map>
#include <memory>


namespace cqde
{

typedef identifier CallbackId;
typedef identifier EntityId;
typedef identifier InputSourceId;
typedef identifier InputDestinationId;
typedef identifier InputCallbackId;
typedef identifier PackageId;
typedef identifier StringId;
typedef identifier TextureId;

namespace types
{

typedef std::map <StringId, std::string> StringStorage;

typedef std::map <TextureId, std::shared_ptr <olc::Renderable>> TextureStorage;

typedef std::map <int32_t, InputSourceId> HwControlMap;

typedef std::map <InputSourceId, InputDestinationId> SwControlMap;

}

} // namespace cqde
