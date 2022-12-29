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

void editorEntityMultipleSelectionEnable(
  entt::registry&,
  const std::vector <std::any>& args = {} );

void editorEntityMultipleSelectionDisable(
  entt::registry&,
  const std::vector <std::any>& args = {} );

void editorEntityMultipleSelectionToggle(
  entt::registry&,
  const std::vector <std::any>& args = {} );

} // cqde::callbacks
