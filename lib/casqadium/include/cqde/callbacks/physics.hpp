#pragma once

#include <entt/fwd.hpp>

#include <any>
#include <vector>


namespace cqde::callbacks
{

void forceEmitterCallback(
  entt::registry&,
  const std::vector <std::any>& args = {} );

void gravityEmitterCallback(
  entt::registry&,
  const std::vector <std::any>& args = {} );

} // cqde::callbacks
