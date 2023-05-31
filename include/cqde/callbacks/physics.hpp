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


void
speedLinearXRelative(
  entt::registry&,
  const std::vector <std::any>& args );

void
speedLinearYRelative(
  entt::registry&,
  const std::vector <std::any>& args );

void
speedLinearZRelative(
  entt::registry&,
  const std::vector <std::any>& args );


void
speedLinearLocalXRelative(
  entt::registry&,
  const std::vector <std::any>& args );

void
speedLinearLocalYRelative(
  entt::registry&,
  const std::vector <std::any>& args );

void
speedLinearLocalZRelative(
  entt::registry&,
  const std::vector <std::any>& args );


void
speedAngularXRelative(
  entt::registry&,
  const std::vector <std::any>& args );

void
speedAngularYRelative(
  entt::registry&,
  const std::vector <std::any>& args );

void
speedAngularZRelative(
  entt::registry&,
  const std::vector <std::any>& args );

} // cqde::callbacks
