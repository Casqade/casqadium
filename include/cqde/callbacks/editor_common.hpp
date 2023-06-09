#pragma once

#include <entt/fwd.hpp>

#include <any>
#include <vector>


namespace cqde::callbacks
{

void editorModeEnable(
  entt::registry&,
  const std::vector <std::any>& args = {} );

void editorModeDisable(
  entt::registry&,
  const std::vector <std::any>& args = {} );

void editorModeToggle(
  entt::registry&,
  const std::vector <std::any>& args = {} );


void editorCameraCreate(
  entt::registry&,
  const std::vector <std::any>& args = {} );

void editorControllerCreate(
  entt::registry&,
  const std::vector <std::any>& args = {} );

void editorBindingsAssign(
  entt::registry&,
  const std::vector <std::any>& args = {} );

void editorEntitySelect(
  entt::registry&,
  const std::vector <std::any>& args = {} );

} // cqde::callbacks
