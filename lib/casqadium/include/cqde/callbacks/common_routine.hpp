#pragma once

#include <entt/fwd.hpp>

#include <any>
#include <vector>


namespace cqde::callbacks
{

void interact(
  entt::registry&,
  const std::vector <std::any>& args = {} );


void entityRemove(
  entt::registry&,
  const std::vector <std::any>& args = {} );


void entityUpdateOn(
  entt::registry&,
  const std::vector <std::any>& args = {} );

void entityUpdateOff(
  entt::registry&,
  const std::vector <std::any>& args = {} );

void entityUpdateToggle(
  entt::registry&,
  const std::vector <std::any>& args = {} );

void entitiesUpdateOn(
  entt::registry&,
  const std::vector <std::any>& args );

void entitiesUpdateOff(
  entt::registry&,
  const std::vector <std::any>& args );


void entityInputOn(
  entt::registry&,
  const std::vector <std::any>& args = {} );

void entityInputOff(
  entt::registry&,
  const std::vector <std::any>& args = {} );

void entityInputToggle(
  entt::registry&,
  const std::vector <std::any>& args = {} );

void entitiesInputOn(
  entt::registry&,
  const std::vector <std::any>& args );

void entitiesInputOff(
  entt::registry&,
  const std::vector <std::any>& args );


void systemsActivate(
  entt::registry&,
  const std::vector <std::any>& args );

void systemsDeactivate(
  entt::registry&,
  const std::vector <std::any>& args );


void quickSave(
  entt::registry&,
  const std::vector <std::any>& args = {} );

void quickLoad(
  entt::registry&,
  const std::vector <std::any>& args = {} );

} // cqde::callbacks
