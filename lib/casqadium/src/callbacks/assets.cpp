#include <cqde/callbacks/assets.hpp>

#include <cqde/types/assets/AudioAssetManager.hpp>
#include <cqde/types/assets/FontAssetManager.hpp>
#include <cqde/types/assets/GeometryAssetManager.hpp>
#include <cqde/types/assets/TerrainAssetManager.hpp>
#include <cqde/types/assets/TextStringAssetManager.hpp>
#include <cqde/types/assets/TextureAssetManager.hpp>

#include <cqde/components/assets/AudioAssetList.hpp>
#include <cqde/components/assets/FontAssetList.hpp>
#include <cqde/components/assets/GeometryAssetList.hpp>
#include <cqde/components/assets/TerrainAssetList.hpp>
#include <cqde/components/assets/TextStringAssetList.hpp>
#include <cqde/components/assets/TextureAssetList.hpp>

#include <entt/entity/registry.hpp>


namespace cqde::callbacks
{

void
loadAudioAssets(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using compos::AudioAssetList;
  using types::AudioAssetManager;

  const auto assetLoadList = std::any_cast <AudioAssetList*> (args.at(0));

  auto& audioManager = registry.ctx().at <AudioAssetManager> ();

  audioManager.load(assetLoadList->audio);
}

void
loadFontAssets(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using compos::FontAssetList;
  using types::FontAssetManager;

  const auto assetLoadList = std::any_cast <FontAssetList*> (args.at(0));

  auto& audioManager = registry.ctx().at <FontAssetManager> ();

  audioManager.load(assetLoadList->fonts);
}

void
loadGeometryAssets(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using compos::GeometryAssetList;
  using types::GeometryAssetManager;

  const auto assetLoadList = std::any_cast <GeometryAssetList*> (args.at(0));

  auto& audioManager = registry.ctx().at <GeometryAssetManager> ();

  audioManager.load(assetLoadList->geometry);
}

void
loadTerrainAssets(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using compos::TerrainAssetList;
  using types::TerrainAssetManager;

  const auto assetLoadList = std::any_cast <TerrainAssetList*> (args.at(0));

  auto& terrainManager = registry.ctx().at <TerrainAssetManager> ();

  terrainManager.load(assetLoadList->terrain);
}

void
loadTextStringAssets(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using compos::TextStringAssetList;
  using types::TextStringAssetManager;

  const auto assetLoadList = std::any_cast <TextStringAssetList*> (args.at(0));

  auto& audioManager = registry.ctx().at <TextStringAssetManager> ();

  audioManager.load(assetLoadList->text);
}

void
loadTextureAssets(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using compos::TextureAssetList;
  using types::TextureAssetManager;

  const auto assetLoadList = std::any_cast <TextureAssetList*> (args.at(0));

  auto& audioManager = registry.ctx().at <TextureAssetManager> ();

  audioManager.load(assetLoadList->textures);
}

void
unloadAudioAssets(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using compos::AudioAssetList;
  using types::AudioAssetManager;

  const auto assetUnloadList = std::any_cast <AudioAssetList*> (args.at(0));

  auto& audioManager = registry.ctx().at <AudioAssetManager> ();

  audioManager.unload(assetUnloadList->audio);
}

void
unloadFontAssets(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using compos::FontAssetList;
  using types::FontAssetManager;

  const auto assetUnloadList = std::any_cast <FontAssetList*> (args.at(0));

  auto& audioManager = registry.ctx().at <FontAssetManager> ();

  audioManager.unload(assetUnloadList->fonts);
}

void
unloadGeometryAssets(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using compos::GeometryAssetList;
  using types::GeometryAssetManager;

  const auto assetUnloadList = std::any_cast <GeometryAssetList*> (args.at(0));

  auto& audioManager = registry.ctx().at <GeometryAssetManager> ();

  audioManager.unload(assetUnloadList->geometry);
}

void
unloadTerrainAssets(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using compos::TerrainAssetList;
  using types::TerrainAssetManager;

  const auto assetUnloadList = std::any_cast <TerrainAssetList*> (args.at(0));

  auto& terrainManager = registry.ctx().at <TerrainAssetManager> ();

  terrainManager.unload(assetUnloadList->terrain);
}

void
unloadTextStringAssets(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using compos::TextStringAssetList;
  using types::TextStringAssetManager;

  const auto assetUnloadList = std::any_cast <TextStringAssetList*> (args.at(0));

  auto& audioManager = registry.ctx().at <TextStringAssetManager> ();

  audioManager.unload(assetUnloadList->text);
}

void
unloadTextureAssets(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using compos::TextureAssetList;
  using types::TextureAssetManager;

  const auto assetUnloadList = std::any_cast <TextureAssetList*> (args.at(0));

  auto& audioManager = registry.ctx().at <TextureAssetManager> ();

  audioManager.unload(assetUnloadList->textures);
}

} // namespace cqde::callbacks
