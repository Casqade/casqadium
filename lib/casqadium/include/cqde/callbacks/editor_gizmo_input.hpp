#pragma once

#include <entt/fwd.hpp>

#include <any>
#include <vector>


namespace cqde::callbacks
{

void editorGizmoSetModeTranslate(
  entt::registry&,
  const std::vector <std::any>& args = {} );

void editorGizmoSetModeRotate(
  entt::registry&,
  const std::vector <std::any>& args = {} );

void editorGizmoSetModeScale(
  entt::registry&,
  const std::vector <std::any>& args = {} );

void editorGizmoSetSpaceLocal(
  entt::registry&,
  const std::vector <std::any>& args = {} );

void editorGizmoSetSpaceWorld(
  entt::registry&,
  const std::vector <std::any>& args = {} );

} // cqde::callbacks
