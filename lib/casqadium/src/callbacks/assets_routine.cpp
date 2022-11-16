#include <cqde/callbacks/assets_routine.hpp>

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

  const auto entity = std::any_cast <entt::entity> (args.at(0));

  auto& assetLoadList = registry.get <const AudioAssetList> (entity);

  auto& audioManager = registry.ctx().at <AudioAssetManager> ();

  audioManager.load(assetLoadList.audio);
}

void
loadFontAssets(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using compos::FontAssetList;
  using types::FontAssetManager;

  const auto entity = std::any_cast <entt::entity> (args.at(0));

  auto& assetLoadList = registry.get <FontAssetList> (entity);

  auto& fontManager = registry.ctx().at <FontAssetManager> ();

  fontManager.load(assetLoadList.fonts);
}

void
loadGeometryAssets(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using compos::GeometryAssetList;
  using types::GeometryAssetManager;

  const auto entity = std::any_cast <entt::entity> (args.at(0));

  auto& assetLoadList = registry.get <GeometryAssetList> (entity);

  auto& geometryManager = registry.ctx().at <GeometryAssetManager> ();

  geometryManager.load(assetLoadList.geometry);
}

void
loadTerrainAssets(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using compos::TerrainAssetList;
  using types::TerrainAssetManager;

  const auto entity = std::any_cast <entt::entity> (args.at(0));

  auto& assetLoadList = registry.get <TerrainAssetList> (entity);

  auto& terrainManager = registry.ctx().at <TerrainAssetManager> ();

  terrainManager.load(assetLoadList.terrain);
}

void
loadTextStringAssets(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using compos::TextStringAssetList;
  using types::TextStringAssetManager;

  const auto entity = std::any_cast <entt::entity> (args.at(0));

  auto& assetLoadList = registry.get <TextStringAssetList> (entity);

  auto& textManager = registry.ctx().at <TextStringAssetManager> ();

  textManager.load(assetLoadList.text);
}

void
loadTextureAssets(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using compos::TextureAssetList;
  using types::TextureAssetManager;

  const auto entity = std::any_cast <entt::entity> (args.at(0));

  auto& assetLoadList = registry.get <TextureAssetList> (entity);

  auto& textureManager = registry.ctx().at <TextureAssetManager> ();

  textureManager.load(assetLoadList.textures);
}

void
unloadAudioAssets(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using compos::AudioAssetList;
  using types::AudioAssetManager;

  const auto entity = std::any_cast <entt::entity> (args.at(0));

  auto& assetUnloadList = registry.get <AudioAssetList> (entity);

  auto& audioManager = registry.ctx().at <AudioAssetManager> ();

  audioManager.unload(assetUnloadList.audio);
}

void
unloadFontAssets(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using compos::FontAssetList;
  using types::FontAssetManager;

  const auto entity = std::any_cast <entt::entity> (args.at(0));

  auto& assetUnloadList = registry.get <FontAssetList> (entity);

  auto& fontManager = registry.ctx().at <FontAssetManager> ();

  fontManager.unload(assetUnloadList.fonts);
}

void
unloadGeometryAssets(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using compos::GeometryAssetList;
  using types::GeometryAssetManager;

  const auto entity = std::any_cast <entt::entity> (args.at(0));

  auto& assetUnloadList = registry.get <GeometryAssetList> (entity);

  auto& geometryManager = registry.ctx().at <GeometryAssetManager> ();

  geometryManager.unload(assetUnloadList.geometry);
}

void
unloadTerrainAssets(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using compos::TerrainAssetList;
  using types::TerrainAssetManager;

  const auto entity = std::any_cast <entt::entity> (args.at(0));

  auto& assetUnloadList = registry.get <TerrainAssetList> (entity);

  auto& terrainManager = registry.ctx().at <TerrainAssetManager> ();

  terrainManager.unload(assetUnloadList.terrain);
}

void
unloadTextStringAssets(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using compos::TextStringAssetList;
  using types::TextStringAssetManager;

  const auto entity = std::any_cast <entt::entity> (args.at(0));

  auto& assetUnloadList = registry.get <TextStringAssetList> (entity);

  auto& textManager = registry.ctx().at <TextStringAssetManager> ();

  textManager.unload(assetUnloadList.text);
}

void
unloadTextureAssets(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using compos::TextureAssetList;
  using types::TextureAssetManager;

  const auto entity = std::any_cast <entt::entity> (args.at(0));

  auto& assetUnloadList = registry.get <TextureAssetList> (entity);

  auto& textureManager = registry.ctx().at <TextureAssetManager> ();

  textureManager.unload(assetUnloadList.textures);
}

} // namespace cqde::callbacks
