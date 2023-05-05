#include <cqde/callbacks/assets_routine.hpp>

#include <cqde/types/assets/AudioAssetManager.hpp>
#include <cqde/types/assets/FontAssetManager.hpp>
#include <cqde/types/assets/MeshAssetManager.hpp>
#include <cqde/types/assets/TerrainAssetManager.hpp>
#include <cqde/types/assets/TextStringAssetManager.hpp>
#include <cqde/types/assets/TextureAssetManager.hpp>

#include <cqde/components/assets/AudioAssetList.hpp>
#include <cqde/components/assets/FontAssetList.hpp>
#include <cqde/components/assets/MeshAssetList.hpp>
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

  auto& assetManager = registry.ctx().get <AudioAssetManager> ();

  assetManager.load(assetLoadList.audio);
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

  auto& assetManager = registry.ctx().get <FontAssetManager> ();

  assetManager.load(assetLoadList.fonts);
}

void
loadMeshAssets(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using compos::MeshAssetList;
  using types::MeshAssetManager;

  const auto entity = std::any_cast <entt::entity> (args.at(0));

  auto& assetLoadList = registry.get <MeshAssetList> (entity);

  auto& assetManager = registry.ctx().get <MeshAssetManager> ();

  assetManager.load(assetLoadList.meshes);
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

  auto& assetManager = registry.ctx().get <TerrainAssetManager> ();

  assetManager.load(assetLoadList.terrain);
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

  auto& assetManager = registry.ctx().get <TextStringAssetManager> ();

  assetManager.load(assetLoadList.text);
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

  auto& assetManager = registry.ctx().get <TextureAssetManager> ();

  assetManager.load(assetLoadList.textures);
}


void
loadAllAudioAssets(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using types::AudioAssetManager;

  auto& assetManager = registry.ctx().get <AudioAssetManager> ();

    for ( const auto& assetId : assetManager.assetIdList() )
      assetManager.load({assetId});
}

void
loadAllFontAssets(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using types::FontAssetManager;

  auto& assetManager = registry.ctx().get <FontAssetManager> ();

    for ( const auto& assetId : assetManager.assetIdList() )
      assetManager.load({assetId});
}

void
loadAllMeshAssets(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using types::MeshAssetManager;

  auto& assetManager = registry.ctx().get <MeshAssetManager> ();

    for ( const auto& assetId : assetManager.assetIdList() )
      assetManager.load({assetId});
}

void
loadAllTerrainAssets(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using types::TerrainAssetManager;

  auto& assetManager = registry.ctx().get <TerrainAssetManager> ();

    for ( const auto& assetId : assetManager.assetIdList() )
      assetManager.load({assetId});
}

void
loadAllTextStringAssets(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using types::TextStringAssetManager;

  auto& assetManager = registry.ctx().get <TextStringAssetManager> ();

    for ( const auto& assetId : assetManager.assetIdList() )
      assetManager.load({assetId});
}

void
loadAllTextureAssets(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using types::TextureAssetManager;

  auto& assetManager = registry.ctx().get <TextureAssetManager> ();

    for ( const auto& assetId : assetManager.assetIdList() )
      assetManager.load({assetId});
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

  auto& assetManager = registry.ctx().get <AudioAssetManager> ();

  assetManager.unload(assetUnloadList.audio);
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

  auto& assetManager = registry.ctx().get <FontAssetManager> ();

  assetManager.unload(assetUnloadList.fonts);
}

void
unloadMeshAssets(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using compos::MeshAssetList;
  using types::MeshAssetManager;

  const auto entity = std::any_cast <entt::entity> (args.at(0));

  auto& assetUnloadList = registry.get <MeshAssetList> (entity);

  auto& assetManager = registry.ctx().get <MeshAssetManager> ();

  assetManager.unload(assetUnloadList.meshes);
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

  auto& assetManager = registry.ctx().get <TerrainAssetManager> ();

  assetManager.unload(assetUnloadList.terrain);
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

  auto& assetManager = registry.ctx().get <TextStringAssetManager> ();

  assetManager.unload(assetUnloadList.text);
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

  auto& assetManager = registry.ctx().get <TextureAssetManager> ();

  assetManager.unload(assetUnloadList.textures);
}


void
waitAudioAssetsLoaded(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using compos::AudioAssetList;
  using types::AudioAssetManager;
  using types::AssetStatus;

  const auto entity = std::any_cast <entt::entity> (args.at(0));

  auto& assetLoadList = registry.get <const AudioAssetList> (entity);

  const auto& assetManager = registry.ctx().get <AudioAssetManager> ();

  for ( const auto& assetId : assetLoadList.audio )
  {
    for ( auto status = AssetStatus::Unloaded;
          status == AssetStatus::Unloaded ||
          status == AssetStatus::Loading;
          status = assetManager.status(assetId) )
      ;
  }
}

void
waitFontAssetsLoaded(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using compos::FontAssetList;
  using types::FontAssetManager;
  using types::AssetStatus;

  const auto entity = std::any_cast <entt::entity> (args.at(0));

  auto& assetLoadList = registry.get <const FontAssetList> (entity);

  const auto& assetManager = registry.ctx().get <FontAssetManager> ();

  for ( const auto& assetId : assetLoadList.fonts )
  {
    for ( auto status = AssetStatus::Unloaded;
          status == AssetStatus::Unloaded ||
          status == AssetStatus::Loading;
          status = assetManager.status(assetId) )
      ;
  }
}

void
waitMeshAssetsLoaded(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using compos::MeshAssetList;
  using types::MeshAssetManager;
  using types::AssetStatus;

  const auto entity = std::any_cast <entt::entity> (args.at(0));

  auto& assetLoadList = registry.get <const MeshAssetList> (entity);

  const auto& assetManager = registry.ctx().get <MeshAssetManager> ();

  for ( const auto& assetId : assetLoadList.meshes )
  {
    for ( auto status = AssetStatus::Unloaded;
          status == AssetStatus::Unloaded ||
          status == AssetStatus::Loading;
          status = assetManager.status(assetId) )
      ;
  }
}

void
waitTerrainAssetsLoaded(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using compos::TerrainAssetList;
  using types::TerrainAssetManager;
  using types::AssetStatus;

  const auto entity = std::any_cast <entt::entity> (args.at(0));

  auto& assetLoadList = registry.get <const TerrainAssetList> (entity);

  const auto& assetManager = registry.ctx().get <TerrainAssetManager> ();

  for ( const auto& assetId : assetLoadList.terrain )
  {
    for ( auto status = AssetStatus::Unloaded;
          status == AssetStatus::Unloaded ||
          status == AssetStatus::Loading;
          status = assetManager.status(assetId) )
      ;
  }
}

void
waitTextStringAssetsLoaded(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using compos::TextStringAssetList;
  using types::TextStringAssetManager;
  using types::AssetStatus;

  const auto entity = std::any_cast <entt::entity> (args.at(0));

  auto& assetLoadList = registry.get <const TextStringAssetList> (entity);

  const auto& assetManager = registry.ctx().get <TextStringAssetManager> ();

  for ( const auto& assetId : assetLoadList.text )
  {
    for ( auto status = AssetStatus::Unloaded;
          status == AssetStatus::Unloaded ||
          status == AssetStatus::Loading;
          status = assetManager.status(assetId) )
      ;
  }
}

void
waitTextureAssetsLoaded(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using compos::TextureAssetList;
  using types::TextureAssetManager;
  using types::AssetStatus;

  const auto entity = std::any_cast <entt::entity> (args.at(0));

  auto& assetLoadList = registry.get <const TextureAssetList> (entity);

  const auto& assetManager = registry.ctx().get <TextureAssetManager> ();

  for ( const auto& assetId : assetLoadList.textures )
  {
    for ( auto status = AssetStatus::Unloaded;
          status == AssetStatus::Unloaded ||
          status == AssetStatus::Loading;
          status = assetManager.status(assetId) )
      ;
  }
}


void
waitAllAudioAssetsLoaded(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using types::AudioAssetManager;
  using types::AssetStatus;

  const auto& assetManager = registry.ctx().get <AudioAssetManager> ();

  for ( const auto& assetId : assetManager.assetIdList() )
  {
    for ( auto status = AssetStatus::Unloaded;
          status == AssetStatus::Unloaded ||
          status == AssetStatus::Loading;
          status = assetManager.status(assetId) )
      ;
  }
}

void
waitAllFontAssetsLoaded(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using types::FontAssetManager;
  using types::AssetStatus;

  const auto& assetManager = registry.ctx().get <FontAssetManager> ();

  for ( const auto& assetId : assetManager.assetIdList() )
  {
    for ( auto status = AssetStatus::Unloaded;
          status == AssetStatus::Unloaded ||
          status == AssetStatus::Loading;
          status = assetManager.status(assetId) )
      ;
  }
}

void
waitAllMeshAssetsLoaded(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using types::MeshAssetManager;
  using types::AssetStatus;

  const auto& assetManager = registry.ctx().get <MeshAssetManager> ();

  for ( const auto& assetId : assetManager.assetIdList() )
  {
    for ( auto status = AssetStatus::Unloaded;
          status == AssetStatus::Unloaded ||
          status == AssetStatus::Loading;
          status = assetManager.status(assetId) )
      ;
  }
}

void
waitAllTerrainAssetsLoaded(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using types::TerrainAssetManager;
  using types::AssetStatus;

  const auto& assetManager = registry.ctx().get <TerrainAssetManager> ();

  for ( const auto& assetId : assetManager.assetIdList() )
  {
    for ( auto status = AssetStatus::Unloaded;
          status == AssetStatus::Unloaded ||
          status == AssetStatus::Loading;
          status = assetManager.status(assetId) )
      ;
  }
}

void
waitAllTextStringAssetsLoaded(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using types::TextStringAssetManager;
  using types::AssetStatus;

  const auto& assetManager = registry.ctx().get <TextStringAssetManager> ();

  for ( const auto& assetId : assetManager.assetIdList() )
  {
    for ( auto status = AssetStatus::Unloaded;
          status == AssetStatus::Unloaded ||
          status == AssetStatus::Loading;
          status = assetManager.status(assetId) )
      ;
  }
}

void
waitAllTextureAssetsLoaded(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using types::TextureAssetManager;
  using types::AssetStatus;

  const auto& assetManager = registry.ctx().get <TextureAssetManager> ();

  for ( const auto& assetId : assetManager.assetIdList() )
  {
    for ( auto status = AssetStatus::Unloaded;
          status == AssetStatus::Unloaded ||
          status == AssetStatus::Loading;
          status = assetManager.status(assetId) )
      ;
  }
}

} // namespace cqde::callbacks
