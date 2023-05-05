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


void loadAllAudioAssets(
  entt::registry&,
  const std::vector <std::any>& args = {} );

void loadAllFontAssets(
  entt::registry&,
  const std::vector <std::any>& args = {} );

void loadAllMeshAssets(
  entt::registry&,
  const std::vector <std::any>& args = {} );

void loadAllTerrainAssets(
  entt::registry&,
  const std::vector <std::any>& args = {} );

void loadAllTextStringAssets(
  entt::registry&,
  const std::vector <std::any>& args = {} );

void loadAllTextureAssets(
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


void waitAudioAssetsLoaded(
  entt::registry&,
  const std::vector <std::any>& args = {} );

void waitFontAssetsLoaded(
  entt::registry&,
  const std::vector <std::any>& args = {} );

void waitMeshAssetsLoaded(
  entt::registry&,
  const std::vector <std::any>& args = {} );

void waitTerrainAssetsLoaded(
  entt::registry&,
  const std::vector <std::any>& args = {} );

void waitTextStringAssetsLoaded(
  entt::registry&,
  const std::vector <std::any>& args = {} );

void waitTextureAssetsLoaded(
  entt::registry&,
  const std::vector <std::any>& args = {} );


void waitAllAudioAssetsLoaded(
  entt::registry&,
  const std::vector <std::any>& args = {} );

void waitAllFontAssetsLoaded(
  entt::registry&,
  const std::vector <std::any>& args = {} );

void waitAllMeshAssetsLoaded(
  entt::registry&,
  const std::vector <std::any>& args = {} );

void waitAllTerrainAssetsLoaded(
  entt::registry&,
  const std::vector <std::any>& args = {} );

void waitAllTextStringAssetsLoaded(
  entt::registry&,
  const std::vector <std::any>& args = {} );

void waitAllTextureAssetsLoaded(
  entt::registry&,
  const std::vector <std::any>& args = {} );

} // cqde::callbacks
