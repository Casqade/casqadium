#pragma once

#include <entt/fwd.hpp>

#include <any>
#include <vector>


namespace cqde::callbacks
{

void audioStopAll(
  entt::registry&,
  const std::vector <std::any>& args = {} );

} // cqde::callbacks
