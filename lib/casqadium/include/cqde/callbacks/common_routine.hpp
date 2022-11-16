#pragma once

#include <entt/fwd.hpp>

#include <any>
#include <vector>


namespace cqde::callbacks
{

void entityUpdateOn(
  entt::registry&,
  const std::vector <std::any>& args = {} );

void entityUpdateOff(
  entt::registry&,
  const std::vector <std::any>& args = {} );

void entityUpdateToggle(
  entt::registry&,
  const std::vector <std::any>& args = {} );

void entityInputOn(
  entt::registry&,
  const std::vector <std::any>& args = {} );

void entityInputOff(
  entt::registry&,
  const std::vector <std::any>& args = {} );

void entityInputToggle(
  entt::registry&,
  const std::vector <std::any>& args = {} );


void quickSave(
  entt::registry&,
  const std::vector <std::any>& args = {} );

void quickLoad(
  entt::registry&,
  const std::vector <std::any>& args = {} );

} // cqde::callbacks
