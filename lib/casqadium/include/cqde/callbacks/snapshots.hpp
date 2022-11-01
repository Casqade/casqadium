#pragma once

#include <entt/fwd.hpp>

#include <any>
#include <vector>


namespace cqde::callbacks
{

void quickSave(
  entt::registry&,
  const std::vector <std::any>& args = {} );

void quickLoad(
  entt::registry&,
  const std::vector <std::any>& args = {} );

} // cqde::callbacks
