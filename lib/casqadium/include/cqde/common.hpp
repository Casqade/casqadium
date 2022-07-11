#pragma once

#include <entt/fwd.hpp>


// assert in debug build
// handle condition safely in release
#define CQDE_ASSERT_DEBUG(cond, action)\
  if (!(cond)) {assert(cond); action;}

namespace cqde
{

void engineInit( entt::registry& registry );

} // namespace cqde
