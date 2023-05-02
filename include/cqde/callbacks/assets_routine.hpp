#pragma once

#include <entt/fwd.hpp>

#include <any>
#include <vector>


namespace cqde::callbacks
{

void loadAudioAssets(
  entt::registry&,
  const std::vector <std::any>& args = {} );

void loadFontAssets(
  entt::registry&,
  const std::vector <std::any>& args = {} );

void loadMeshAssets(
  entt::registry&,
  const std::vector <std::any>& args = {} );

void loadTerrainAssets(
  entt::registry&,
  const std::vector <std::any>& args = {} );

void loadTextStringAssets(
  entt::registry&,
  const std::vector <std::any>& args = {} );

void loadTextureAssets(
  entt::registry&,
  const std::vector <std::any>& args = {} );


void unloadAudioAssets(
  entt::registry&,
  const std::vector <std::any>& args = {} );

void unloadFontAssets(
  entt::registry&,
  const std::vector <std::any>& args = {} );

void unloadMeshAssets(
  entt::registry&,
  const std::vector <std::any>& args = {} );

void unloadTerrainAssets(
  entt::registry&,
  const std::vector <std::any>& args = {} );

void unloadTextStringAssets(
  entt::registry&,
  const std::vector <std::any>& args = {} );

void unloadTextureAssets(
  entt::registry&,
  const std::vector <std::any>& args = {} );

} // cqde::callbacks
