#pragma once

#include <entt/fwd.hpp>

#include <any>
#include <vector>


namespace cqde::callbacks
{

void interact(
  entt::registry&,
  const std::vector <std::any>& args = {} );

void interactProbe(
  entt::registry&,
  const std::vector <std::any>& args = {} );


void entityRemove(
  entt::registry&,
  const std::vector <std::any>& args = {} );

void entitiesRemove(
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

void entitiesUpdateToggle(
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

void entitiesInputToggle(
  entt::registry&,
  const std::vector <std::any>& args );


void sceneNodeRemove(
  entt::registry&,
  const std::vector <std::any>& args = {} );

void sceneNodesRemove(
  entt::registry&,
  const std::vector <std::any>& args = {} );


void systemsActivate(
  entt::registry&,
  const std::vector <std::any>& args );

void systemsDeactivate(
  entt::registry&,
  const std::vector <std::any>& args );

void systemsToggle(
  entt::registry&,
  const std::vector <std::any>& args );


void quickSave(
  entt::registry&,
  const std::vector <std::any>& args = {} );

void quickLoad(
  entt::registry&,
  const std::vector <std::any>& args = {} );

void engineShutdown(
  entt::registry&,
  const std::vector <std::any>& args = {} );

} // cqde::callbacks
