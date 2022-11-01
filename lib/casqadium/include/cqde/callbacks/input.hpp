#pragma once

#include <entt/fwd.hpp>

#include <any>
#include <vector>


namespace cqde::callbacks
{

void mouseAutoCenterEnable(
  entt::registry&,
  const std::vector <std::any>& args = {} );

void mouseAutoCenterDisable(
  entt::registry&,
  const std::vector <std::any>& args = {} );

void mouseAutoCenterToggle(
  entt::registry&,
  const std::vector <std::any>& args = {} );

void mouseCursorHide(
  entt::registry&,
  const std::vector <std::any>& args = {} );

void mouseCursorShow(
  entt::registry&,
  const std::vector <std::any>& args = {} );

void mouseGrabToggle(
  entt::registry&,
  const std::vector <std::any>& args = {} );


void controlTranslateXRelative(
  entt::registry&,
  const std::vector <std::any>& args = {} );

void controlTranslateYRelative(
  entt::registry&,
  const std::vector <std::any>& args = {} );

void controlTranslateZRelative(
  entt::registry&,
  const std::vector <std::any>& args = {} );

void controlPitchRelative(
  entt::registry&,
  const std::vector <std::any>& args = {} );

void controlYawRelative(
  entt::registry&,
  const std::vector <std::any>& args = {} );

void controlRollRelative(
  entt::registry&,
  const std::vector <std::any>& args = {} );

} // cqde::callbacks
