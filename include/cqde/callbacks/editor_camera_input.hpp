#pragma once

#include <entt/fwd.hpp>

#include <any>
#include <vector>


namespace cqde::callbacks
{

void editorCameraControlOn(
  entt::registry&,
  const std::vector <std::any>& args = {} );

void editorCameraControlOff(
  entt::registry&,
  const std::vector <std::any>& args = {} );

void editorCameraFovControl(
  entt::registry&,
  const std::vector <std::any>& args = {} );

void editorCameraZoomControl(
  entt::registry&,
  const std::vector <std::any>& args = {} );

void editorCameraSpeedControl(
  entt::registry&,
  const std::vector <std::any>& args = {} );


void editorCameraTranslateXRelative(
  entt::registry&,
  const std::vector <std::any>& args = {} );

void editorCameraTranslateYRelative(
  entt::registry&,
  const std::vector <std::any>& args = {} );

void editorCameraTranslateZRelative(
  entt::registry&,
  const std::vector <std::any>& args = {} );

} // cqde::callbacks
