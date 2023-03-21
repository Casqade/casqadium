#pragma once

#include <entt/fwd.hpp>

#include <any>
#include <vector>


namespace cqde::callbacks
{

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
