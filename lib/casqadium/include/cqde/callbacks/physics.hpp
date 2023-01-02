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


void
forceXRelative(
  entt::registry&,
  const std::vector <std::any>& args );

void
forceYRelative(
  entt::registry&,
  const std::vector <std::any>& args );

void
forceZRelative(
  entt::registry&,
  const std::vector <std::any>& args );


void
torqueXRelative(
  entt::registry&,
  const std::vector <std::any>& args );

void
torqueYRelative(
  entt::registry&,
  const std::vector <std::any>& args );

void
torqueZRelative(
  entt::registry&,
  const std::vector <std::any>& args );

} // cqde::callbacks
