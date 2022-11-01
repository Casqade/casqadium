#include <cqde/callbacks/assets.hpp>

#include <cqde/types/assets/AudioAssetManager.hpp>
#include <cqde/types/assets/FontAssetManager.hpp>
#include <cqde/types/assets/GeometryAssetManager.hpp>
#include <cqde/types/assets/TerrainAssetManager.hpp>
#include <cqde/types/assets/TextStringAssetManager.hpp>
#include <cqde/types/assets/TextureAssetManager.hpp>

#include <cqde/components/assets/AudioAssetLoadList.hpp>
#include <cqde/components/assets/AudioAssetUnloadList.hpp>
#include <cqde/components/assets/FontAssetLoadList.hpp>
#include <cqde/components/assets/FontAssetUnloadList.hpp>
#include <cqde/components/assets/GeometryAssetLoadList.hpp>
#include <cqde/components/assets/GeometryAssetUnloadList.hpp>
#include <cqde/components/assets/TerrainAssetLoadList.hpp>
#include <cqde/components/assets/TerrainAssetUnloadList.hpp>
#include <cqde/components/assets/TextStringAssetLoadList.hpp>
#include <cqde/components/assets/TextStringAssetUnloadList.hpp>
#include <cqde/components/assets/TextureAssetLoadList.hpp>
#include <cqde/components/assets/TextureAssetUnloadList.hpp>

#include <entt/entity/registry.hpp>


namespace cqde::callbacks
{

void
loadAudioAssets(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using compos::AudioAssetLoadList;
  using types::AudioAssetManager;

  const auto assetLoadList = std::any_cast <AudioAssetLoadList*> (args.at(0));

  auto& audioManager = registry.ctx().at <AudioAssetManager> ();

  audioManager.load(assetLoadList->audioToLoad);
}

void
loadFontAssets(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using compos::FontAssetLoadList;
  using types::FontAssetManager;

  const auto assetLoadList = std::any_cast <FontAssetLoadList*> (args.at(0));

  auto& audioManager = registry.ctx().at <FontAssetManager> ();

  audioManager.load(assetLoadList->fontsToLoad);
}

void
loadGeometryAssets(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using compos::GeometryAssetLoadList;
  using types::GeometryAssetManager;

  const auto assetLoadList = std::any_cast <GeometryAssetLoadList*> (args.at(0));

  auto& audioManager = registry.ctx().at <GeometryAssetManager> ();

  audioManager.load(assetLoadList->geometryToLoad);
}

void
loadTerrainAssets(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using compos::TerrainAssetLoadList;
  using types::TerrainAssetManager;

  const auto assetLoadList = std::any_cast <TerrainAssetLoadList*> (args.at(0));

  auto& terrainManager = registry.ctx().at <TerrainAssetManager> ();

  terrainManager.load(assetLoadList->terrainToLoad);
}

void
loadTextStringAssets(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using compos::TextStringAssetLoadList;
  using types::TextStringAssetManager;

  const auto assetLoadList = std::any_cast <TextStringAssetLoadList*> (args.at(0));

  auto& audioManager = registry.ctx().at <TextStringAssetManager> ();

  audioManager.load(assetLoadList->textToLoad);
}

void
loadTextureAssets(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using compos::TextureAssetLoadList;
  using types::TextureAssetManager;

  const auto assetLoadList = std::any_cast <TextureAssetLoadList*> (args.at(0));

  auto& audioManager = registry.ctx().at <TextureAssetManager> ();

  audioManager.load(assetLoadList->texturesToLoad);
}

void
unloadAudioAssets(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using compos::AudioAssetUnloadList;
  using types::AudioAssetManager;

  const auto assetUnloadList = std::any_cast <AudioAssetUnloadList*> (args.at(0));

  auto& audioManager = registry.ctx().at <AudioAssetManager> ();

  audioManager.unload(assetUnloadList->audioToUnload);
}

void
unloadFontAssets(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using compos::FontAssetUnloadList;
  using types::FontAssetManager;

  const auto assetUnloadList = std::any_cast <FontAssetUnloadList*> (args.at(0));

  auto& audioManager = registry.ctx().at <FontAssetManager> ();

  audioManager.unload(assetUnloadList->fontsToUnload);
}

void
unloadGeometryAssets(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using compos::GeometryAssetUnloadList;
  using types::GeometryAssetManager;

  const auto assetUnloadList = std::any_cast <GeometryAssetUnloadList*> (args.at(0));

  auto& audioManager = registry.ctx().at <GeometryAssetManager> ();

  audioManager.unload(assetUnloadList->geometryToUnload);
}

void
unloadTerrainAssets(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using compos::TerrainAssetUnloadList;
  using types::TerrainAssetManager;

  const auto assetUnloadList = std::any_cast <TerrainAssetUnloadList*> (args.at(0));

  auto& terrainManager = registry.ctx().at <TerrainAssetManager> ();

  terrainManager.unload(assetUnloadList->terrainToUnload);
}

void
unloadTextStringAssets(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using compos::TextStringAssetUnloadList;
  using types::TextStringAssetManager;

  const auto assetUnloadList = std::any_cast <TextStringAssetUnloadList*> (args.at(0));

  auto& audioManager = registry.ctx().at <TextStringAssetManager> ();

  audioManager.unload(assetUnloadList->textToUnload);
}

void
unloadTextureAssets(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using compos::TextureAssetUnloadList;
  using types::TextureAssetManager;

  const auto assetUnloadList = std::any_cast <TextureAssetUnloadList*> (args.at(0));

  auto& audioManager = registry.ctx().at <TextureAssetManager> ();

  audioManager.unload(assetUnloadList->texturesToUnload);
}

} // namespace cqde::callbacks
