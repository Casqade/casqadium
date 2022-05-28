#pragma once

#include <cqde/fwd.hpp>
#include <cqde/identifier.hpp>

#include <olcPGE/olcPixelGameEngine.hpp>

#include <map>
#include <memory>
#include <set>


namespace cqde
{

typedef identifier CallbackId;
typedef identifier EntityId;
typedef identifier InputAxisId;
typedef identifier InputCallbackId;
typedef identifier InputHwId;
typedef identifier PackageId;
typedef identifier StringId;
typedef identifier TextureId;

namespace types
{

typedef std::map <StringId, std::string> StringStorage;

typedef std::map <TextureId, std::shared_ptr <olc::Renderable>> TextureStorage;

typedef std::map <int32_t, InputHwId> HwControlMap;

typedef std::multimap < std::shared_ptr <InputBinding>, InputAxisId,
                        InputBindingComparator> InputBindings;

}

} // namespace cqde
