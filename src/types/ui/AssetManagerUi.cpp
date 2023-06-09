#include <cqde/types/ui/AssetManagerUi.hpp>
#include <cqde/types/PackageManager.hpp>

#include <cqde/types/assets/AudioAssetManager.hpp>
#include <cqde/types/assets/FontAssetManager.hpp>
#include <cqde/types/assets/MeshAssetManager.hpp>
#include <cqde/types/assets/MouseCursorAssetManager.hpp>
#include <cqde/types/assets/TextStringAssetManager.hpp>
#include <cqde/types/assets/TerrainAssetManager.hpp>
#include <cqde/types/assets/TextureAssetManager.hpp>

#include <cqde/assert.hpp>
#include <cqde/file_helpers.hpp>
#include <cqde/json_helpers.hpp>
#include <cqde/logger.hpp>

#include <entt/entity/registry.hpp>

#include <imgui.h>
#include <imgui_stdlib.h>


namespace cqde::ui
{

AssetManagerUi::AssetManagerUi()
{
  mAssetTypeNames =
  {
    {ContentType::Audio, "Audio"},
    {ContentType::Fonts, "Fonts"},
    {ContentType::Meshes, "Meshes"},
    {ContentType::MouseCursors, "Mouse cursors"},
    {ContentType::Terrain, "Terrain"},
    {ContentType::Text, "Text"},
    {ContentType::Textures, "Textures"},
  };

  mSelectedAssetIds =
  {
    {ContentType::Audio, {}},
    {ContentType::Fonts, {}},
    {ContentType::Meshes, {}},
    {ContentType::MouseCursors, {}},
    {ContentType::Terrain, {}},
    {ContentType::Text, {}},
    {ContentType::Textures, {}},
  };

  mSelectedAssetType = mAssetTypeNames.begin()->first;
}

void
AssetManagerUi::stateApply(
  entt::registry& registry )
{
  using AudioAssetManager = types::AudioAssetManager;
  using FontAssetManager = types::FontAssetManager;
  using MeshAssetManager = types::MeshAssetManager;
  using MouseCursorAssetManager = types::MouseCursorAssetManager;
  using TerrainAssetManager = types::TerrainAssetManager;
  using TextStringAssetManager = types::TextStringAssetManager;
  using TextureAssetManager = types::TextureAssetManager;

  using PackageManager = types::PackageManager;
  using ContentType = types::Package::ContentType;

  auto& audioMgr = registry.ctx().get <AudioAssetManager> ();
  auto& fontMgr = registry.ctx().get <FontAssetManager> ();
  auto& meshesMgr = registry.ctx().get <MeshAssetManager> ();
  auto& mouseCursorMgr = registry.ctx().get <MouseCursorAssetManager> ();
  auto& terrainMgr = registry.ctx().get <TerrainAssetManager> ();
  auto& textMgr = registry.ctx().get <TextStringAssetManager> ();
  auto& textureMgr = registry.ctx().get <TextureAssetManager> ();

  mAssetsState.clear();

  audioMgr.clear();
  fontMgr.clear();
  meshesMgr.clear();
  terrainMgr.clear();
  textMgr.clear();
  textureMgr.clear();

  const auto& pkgMgr = registry.ctx().get <PackageManager> ();

  for ( const auto& packageId : pkgMgr.packages() )
  {
    auto package = pkgMgr.package(packageId);
    CQDE_ASSERT_DEBUG(package != nullptr, continue);

    const auto audioPath = package->contentPath(ContentType::Audio);
    const auto fontsPath = package->contentPath(ContentType::Fonts);
    const auto meshesPath = package->contentPath(ContentType::Meshes);
    const auto mouseCursorPath = package->contentPath(ContentType::MouseCursors);
    const auto terrainPath = package->contentPath(ContentType::Terrain);
    const auto textPath = package->contentPath(ContentType::Text);
    const auto texturesPath = package->contentPath(ContentType::Textures);

    audioMgr.parseAssetDbFile(audioPath);
    fontMgr.parseAssetDbFile(fontsPath);
    meshesMgr.parseAssetDbFile(meshesPath);
    mouseCursorMgr.parseAssetDbFile(mouseCursorPath);
    terrainMgr.parseAssetDbFile(terrainPath);
    textMgr.parseAssetDbFile(textPath);
    textureMgr.parseAssetDbFile(texturesPath);
  }
}

void
AssetManagerUi::stateSave(
  const PackageId& packageId,
  entt::registry& registry )
{
  using types::PackageManager;
  using ContentType = types::Package::ContentType;

  CQDE_ASSERT_DEBUG(packageId.str().empty() == false, return);

  LOG_TRACE("Writing package '{}' input config", packageId.str());

  auto& pkgMgr = registry.ctx().get <PackageManager> ();

  auto package = pkgMgr.package(packageId);

  if ( package == nullptr )
  {
    LOG_ERROR("Failed to write package '{}' input config: "
              "No such package in PackageManager",
              packageId.str());
    return;
  }

  const auto& audioDb = mAssetsState[packageId][mAssetTypeNames.at(ContentType::Audio)];
  const auto& fontsDb = mAssetsState[packageId][mAssetTypeNames.at(ContentType::Fonts)];
  const auto& meshesDb = mAssetsState[packageId][mAssetTypeNames.at(ContentType::Meshes)];
  const auto& mouseCursorsDb = mAssetsState[packageId][mAssetTypeNames.at(ContentType::MouseCursors)];
  const auto& terrainDb = mAssetsState[packageId][mAssetTypeNames.at(ContentType::Terrain)];
  const auto& textDb = mAssetsState[packageId][mAssetTypeNames.at(ContentType::Text)];
  const auto& texturesDb = mAssetsState[packageId][mAssetTypeNames.at(ContentType::Textures)];

  if ( audioDb.isNull() == false )
    package->save(ContentType::Audio, audioDb);

  if ( fontsDb.isNull() == false )
    package->save(ContentType::Fonts, fontsDb);

  if ( meshesDb.isNull() == false )
    package->save(ContentType::Meshes, meshesDb);

  if ( mouseCursorsDb.isNull() == false )
    package->save(ContentType::MouseCursors, mouseCursorsDb);

  if ( terrainDb.isNull() == false )
    package->save(ContentType::Terrain, terrainDb);

  if ( textDb.isNull() == false )
    package->save(ContentType::Text, textDb);

  if ( texturesDb.isNull() == false )
    package->save(ContentType::Textures, texturesDb);
}

void
AssetManagerUi::ui_show(
  entt::registry& registry )
{
  using PackageManager = types::PackageManager;

  if ( ImGui::Begin("Assets", nullptr,
                    ImGuiWindowFlags_MenuBar) == false )
  {
    ImGui::End(); // Assets
    return;
  }

  if ( ImGui::CollapsingHeader("Filter", ImGuiTreeNodeFlags_DefaultOpen) )
  {
    ImGui::AlignTextToFramePadding();
    ImGui::Text("Package:");

    ImGui::SameLine();
    mPackageFilter.select(registry);

    if ( mPackageFilter.package().str().empty() == true &&
         ImGui::IsItemHovered() )
      ImGui::SetTooltip("Live assets state");

    ImGui::AlignTextToFramePadding();
    ImGui::Text("Type:");

    auto assetTypeName = mAssetTypeNames.at(mSelectedAssetType);

    ImGui::SameLine();
    if ( ImGui::BeginCombo("##assetTypeCombo", assetTypeName.c_str() ) )
    {
      for ( const auto& [assetType, assetTypeName] : mAssetTypeNames )
        if ( ImGui::Selectable(assetTypeName.c_str(), assetType == mSelectedAssetType) )
          mSelectedAssetType = assetType;

      ImGui::EndCombo();
    }

    mAssetIdFilter.search();
  }

  ImGui::Separator();

  const auto& pkgMgr = registry.ctx().get <PackageManager> ();

  auto package = pkgMgr.package(mPackageFilter.package());

  ui_show_menu_bar(registry);

  if ( package == nullptr )
  {
    ui_show_live_state(registry);
    return ImGui::End(); // Assets
  }

  ui_show_package_state(registry);

  ImGui::End(); // Assets

  ui_show_asset_window(registry);

  mHistoryBuffer.push(mAssetsState);
}

void
AssetManagerUi::ui_show_live_state(
  entt::registry& registry )
{
  const auto tableFlags = ImGuiTableFlags_ScrollX |
                          ImGuiTableFlags_ScrollY;

  if ( ImGui::BeginTable( "AssetsLiveList", 1, tableFlags) == false )
    return;

  ImGui::TableNextColumn();

  switch (mSelectedAssetType)
  {
    case ContentType::Audio:
    {
      ui_show_live_audio(registry);
      break;
    }
    case ContentType::Fonts:
    {
      ui_show_live_font(registry);
      break;
    }
    case ContentType::Meshes:
    {
      ui_show_live_meshes(registry);
      break;
    }
    case ContentType::MouseCursors:
    {
      ui_show_live_mouse_cursors(registry);
      break;
    }

    case ContentType::Terrain:
    {
      ui_show_live_terrain(registry);
      break;
    }
    case ContentType::Text:
    {
      ui_show_live_text(registry);
      break;
    }
    case ContentType::Textures:
    {
      ui_show_live_texture(registry);
      break;
    }
    default:
      CQDE_ASSERT_DEBUG(false, break);
  }

  ImGui::EndTable(); // AssetsLiveList
}

void
AssetManagerUi::ui_show_package_state(
  entt::registry& registry )
{
  using fmt::format;

  using PackageManager = types::PackageManager;

  using AudioAssetManager = types::AudioAssetManager;
  using FontAssetManager = types::FontAssetManager;
  using MeshAssetManager = types::MeshAssetManager;
  using MouseCursorAssetManager = types::MouseCursorAssetManager;
  using TerrainAssetManager = types::TerrainAssetManager;
  using TextStringAssetManager = types::TextStringAssetManager;
  using TextureAssetManager = types::TextureAssetManager;

  const auto assetTypeName = mAssetTypeNames.at(mSelectedAssetType);

  auto& assetDb = mAssetsState[mPackageFilter.package()][assetTypeName];

  if ( assetDb.isNull() == true )
  {
    const auto& pkgMgr = registry.ctx().get <PackageManager> ();

    auto package = pkgMgr.package(mPackageFilter.package());

    CQDE_ASSERT_DEBUG(package != nullptr, return);

    const auto assetDbPath = package->contentPath(mSelectedAssetType);

    if ( fileExists(assetDbPath) == true )
      assetDb = fileParse(assetDbPath);
    else
      assetDb = Json::objectValue;

    auto configIter = mHistoryBuffer.current();
    if ( mHistoryBuffer.isValid(configIter) == true )
      (*configIter)[mPackageFilter.package()][assetTypeName] = assetDb;
  }

  switch (mSelectedAssetType)
  {
    case ContentType::Audio:
    {
      AudioAssetManager::Validate(assetDb);
      break;
    }

    case ContentType::Fonts:
    {
      FontAssetManager::Validate(assetDb);
      break;
    }

    case ContentType::Meshes:
    {
      MeshAssetManager::Validate(assetDb);
      break;
    }

    case ContentType::MouseCursors:
    {
      MouseCursorAssetManager::Validate(assetDb);
      break;
    }

    case ContentType::Terrain:
    {
      TerrainAssetManager::Validate(assetDb);
      break;
    }

    case ContentType::Text:
    {
      TextStringAssetManager::Validate(assetDb);
      break;
    }

    case ContentType::Textures:
    {
      TextureAssetManager::Validate(assetDb);
      break;
    }

    default:
      CQDE_ASSERT_DEBUG(false, return);
  }

  ImGui::BeginDisabled(mNewAssetName.empty() == true ||
                       assetDb.isMember(mNewAssetName) == true);

  const bool newAssetInserted = ImGui::Button("+##assetAdd");

  ImGui::EndDisabled();

  ImGui::SameLine();
  ImGui::InputTextWithHint("##newAssetId", "New asset ID", &mNewAssetName,
                           ImGuiInputTextFlags_AutoSelectAll);

  if ( newAssetInserted == true )
  {
    Json::Value newAsset {};

    switch (mSelectedAssetType)
    {
      case ContentType::Audio:
      {
        newAsset = AudioAssetManager::AssetJsonDbEntryReference();
        break;
      }

      case ContentType::Fonts:
      {
        newAsset = FontAssetManager::AssetJsonDbEntryReference();
        break;
      }

      case ContentType::Meshes:
      {
        newAsset = MeshAssetManager::AssetJsonDbEntryReference();
        break;
      }

      case ContentType::MouseCursors:
      {
        newAsset = MouseCursorAssetManager::AssetJsonDbEntryReference();
        break;
      }

      case ContentType::Terrain:
      {
        newAsset = TerrainAssetManager::AssetJsonDbEntryReference();
        break;
      }

      case ContentType::Text:
      {
        newAsset = TextStringAssetManager::AssetJsonDbEntryReference();
        break;
      }

      case ContentType::Textures:
      {
        newAsset = TextureAssetManager::AssetJsonDbEntryReference();
        break;
      }

      defaut:
        CQDE_ASSERT_DEBUG(false, return);
        break;
    }

    jsonClearComments(newAsset);
    assetDb[mNewAssetName] = newAsset;
  }

  ImGui::Separator();

  const auto tableFlags = ImGuiTableFlags_ScrollX |
                          ImGuiTableFlags_ScrollY;

  if ( ImGui::BeginTable( "AssetsPackageList", 1, tableFlags) == false )
    return;

  ImGui::TableNextColumn();

  for ( const auto& asset : assetDb.getMemberNames() )
  {
    if ( assetDb.isMember(asset) == false )
      continue; // handle assets removed during loop

    if ( mAssetIdFilter.query(asset) == false )
      continue;

    ImGui::PushID(asset.c_str());

    if ( ImGui::SmallButton("-##assetRemove") )
    {
      if ( asset == mSelectedAssetIds[mSelectedAssetType] )
        mSelectedAssetIds[mSelectedAssetType].clear();

      assetDb.removeMember(asset);

      ImGui::PopID();
      continue;
    }

    const bool selected = asset == mSelectedAssetIds[mSelectedAssetType];

    const auto flags =  ImGuiSelectableFlags_SpanAllColumns |
                        ImGuiSelectableFlags_AllowItemOverlap;

    ImGui::SameLine();
    if ( ImGui::Selectable( asset.c_str(),
                            selected, flags ) )
    {
      mSelectedAssetIds[mSelectedAssetType] = asset;

      mAssetWindowOpened = true;
      ImGui::SetWindowFocus("###assetEditWindow");
    }

    if ( ImGui::IsItemHovered() &&
         ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left) )
    {
      mRenamedAssetId = asset;
      ImGui::OpenPopup("##assetRenamePopup");
    }

    if ( ImGui::BeginPopup("##assetRenamePopup") )
    {
      ImGui::SetKeyboardFocusHere();

      ImGui::Text("Rename asset:");

      const bool assetRenamed = ImGui::InputText("##assetRename", &mRenamedAssetId,
                                                  ImGuiInputTextFlags_AutoSelectAll |
                                                  ImGuiInputTextFlags_EnterReturnsTrue);

      const bool assetExists =  mRenamedAssetId != asset &&
                                assetDb.isMember(mRenamedAssetId);

      if ( assetExists == true )
        ImGui::TextColored(ImVec4{1.0f, 0.0f, 0.0f, 1.0f},
                           "%s", format("Asset '{}' already exists",
                                        mRenamedAssetId).c_str());

      if ( mRenamedAssetId == null_id.str() )
        ImGui::TextColored(ImVec4{1.0f, 0.0f, 0.0f, 1.0f},
                           "Invalid asset ID");

      if ( assetRenamed == true &&
           mRenamedAssetId != null_id.str() )
      {
        if ( assetExists == false )
          ImGui::CloseCurrentPopup();

        if ( assetDb.isMember(mRenamedAssetId) == false )
        {
          assetDb[mRenamedAssetId] = assetDb[asset];
          assetDb.removeMember(asset);

          if ( mSelectedAssetIds[mSelectedAssetType] == asset )
            mSelectedAssetIds[mSelectedAssetType] = mRenamedAssetId;
        }
      }
      ImGui::EndPopup();
    }

    ImGui::PopID();
  }

  ImGui::EndTable(); // AssetsPackageList
}

void
AssetManagerUi::ui_show_menu_bar(
  entt::registry& registry )
{
  using fmt::format;

  if ( ImGui::BeginMenuBar() == false )
    return;

  const auto selectedPackage = mPackageFilter.package();

  if ( ImGui::BeginMenu("Save") )
  {
    if ( selectedPackage.str().empty() == false &&
         ImGui::MenuItem(format("Save '{}'", selectedPackage.str()).c_str()) )
      stateSave(selectedPackage, registry);

    if ( ImGui::MenuItem("Save all") )
      for ( const auto& [packageId, state] : mAssetsState )
        if ( packageId.str().empty() == false )
          stateSave(packageId, registry);

    if ( ImGui::MenuItem("Save & apply") )
    {
      for ( const auto& [packageId, state] : mAssetsState )
        if ( packageId.str().empty() == false )
          stateSave(packageId, registry);

      stateApply(registry);
    }

    ImGui::EndMenu(); // Save
  }

  if ( ImGui::BeginMenu("Load") )
  {
    if ( selectedPackage.str().empty() == false &&
         ImGui::MenuItem(format("Load '{}'", selectedPackage.str()).c_str()) )
      mAssetsState.erase(selectedPackage);

    if ( ImGui::MenuItem("Load all") )
      mAssetsState.clear();

    if ( ImGui::MenuItem("Load & apply") )
      stateApply(registry);

    ImGui::EndMenu(); // Load
  }

  ImGui::BeginDisabled(mHistoryBuffer.undoAvailable() == false);

  if ( ImGui::MenuItem("Undo") )
    mAssetsState = mHistoryBuffer.undo();

  ImGui::EndDisabled();

  ImGui::BeginDisabled(mHistoryBuffer.redoAvailable() == false);

  if ( ImGui::MenuItem("Redo") )
    mAssetsState = mHistoryBuffer.redo();

  ImGui::EndDisabled();

  ImGui::EndMenuBar();
}

void
AssetManagerUi::ui_show_asset_window(
  entt::registry& registry )
{
  using fmt::format;

  using AudioAssetManager = types::AudioAssetManager;
  using FontAssetManager = types::FontAssetManager;
  using MeshAssetManager = types::MeshAssetManager;
  using MouseCursorAssetManager = types::MouseCursorAssetManager;
  using TerrainAssetManager = types::TerrainAssetManager;
  using TextStringAssetManager = types::TextStringAssetManager;
  using TextureAssetManager = types::TextureAssetManager;

  const auto selectedAssetTypeName = mAssetTypeNames.at(mSelectedAssetType);
  const auto selectedAssetId = mSelectedAssetIds.at(mSelectedAssetType);

  if ( selectedAssetId.empty() == true )
    return;

  if ( mAssetWindowOpened == false )
    mSelectedAssetIds[mSelectedAssetType].clear();

  auto& assetDb = mAssetsState[mPackageFilter.package()][selectedAssetTypeName];

  if ( assetDb.isMember(selectedAssetId) == false )
    return;

  const auto windowTitle = format("Asset '{}'###assetEditWindow", selectedAssetId);

  if ( ImGui::Begin(windowTitle.c_str(), &mAssetWindowOpened,
                    ImGuiWindowFlags_HorizontalScrollbar) == false )
    return ImGui::End(); // windowTitle

  auto& assetEntry = assetDb[selectedAssetId];

  ImGui::PushID(selectedAssetId.c_str());

  switch (mSelectedAssetType)
  {
    case ContentType::Audio:
    {
      registry.ctx().get <AudioAssetManager> ().ui_show(assetEntry);
      break;
    }
    case ContentType::Fonts:
    {
      registry.ctx().get <FontAssetManager> ().ui_show(assetEntry);
      break;
    }
    case ContentType::Meshes:
    {
      registry.ctx().get <MeshAssetManager> ().ui_show(assetEntry);
      break;
    }
    case ContentType::MouseCursors:
    {
      registry.ctx().get <MouseCursorAssetManager> ().ui_show(assetEntry);
      break;
    }
    case ContentType::Terrain:
    {
      registry.ctx().get <TerrainAssetManager> ().ui_show(assetEntry);
      break;
    }
    case ContentType::Text:
    {
      registry.ctx().get <TextStringAssetManager> ().ui_show(assetEntry);
      break;
    }
    case ContentType::Textures:
    {
      registry.ctx().get <TextureAssetManager> ().ui_show(assetEntry);
      break;
    }
    default:
    {
      ImGui::PopID();
      CQDE_ASSERT_DEBUG(false, return);
    }
  }

  ImGui::PopID();

  ImGui::End(); // windowTitle
}

void
AssetManagerUi::ui_show_live_audio(
  entt::registry& registry )
{
  using fmt::format;
  using AudioAssetManager = types::AudioAssetManager;

  auto& audioMgr = registry.ctx().get <AudioAssetManager> ();
  auto assets = audioMgr.assetIdList();

  for ( const auto& asset : assets )
  {
    if ( mAssetIdFilter.query(asset.str()) == false )
      continue;

    const bool selected = asset.str() == mSelectedAssetIds[mSelectedAssetType];

    const auto flags =  ImGuiSelectableFlags_SpanAllColumns |
                        ImGuiSelectableFlags_AllowItemOverlap;

    if ( ImGui::Selectable( asset.str().c_str(),
                            selected, flags ) )
    {
      mSelectedAssetIds[mSelectedAssetType] = asset.str();

      mAssetWindowOpened = true;
      ImGui::SetWindowFocus("###assetEditWindow");
    }
  }

  if ( mAssetWindowOpened == false )
    return;

  const auto& selectedAudioId = mSelectedAssetIds[mSelectedAssetType];

  if ( selectedAudioId.empty() == true )
    return;

  const auto windowTitle = format("Audio '{}'###assetEditWindow",
                                  mSelectedAssetIds[mSelectedAssetType]);

  if ( ImGui::Begin(windowTitle.c_str(), &mAssetWindowOpened,
                    ImGuiWindowFlags_HorizontalScrollbar) == true )
  {
    if ( audioMgr.status(selectedAudioId) != AssetStatus::Undefined )
      audioMgr.load({selectedAudioId});

    audioMgr.ui_show_preview(selectedAudioId, registry);
  }

  ImGui::End(); // windowTitle
}

void
AssetManagerUi::ui_show_live_font(
  entt::registry& registry )
{
  using fmt::format;
  using FontAssetManager = types::FontAssetManager;

  auto& fontMgr = registry.ctx().get <FontAssetManager> ();
  auto assets = fontMgr.assetIdList();

  for ( const auto& asset : assets )
  {
    if ( mAssetIdFilter.query(asset.str()) == false )
      continue;

    const bool selected = asset.str() == mSelectedAssetIds[mSelectedAssetType];

    const auto flags =  ImGuiSelectableFlags_SpanAllColumns |
                        ImGuiSelectableFlags_AllowItemOverlap;

    if ( ImGui::Selectable( asset.str().c_str(),
                            selected, flags ) )
    {
      mSelectedAssetIds[mSelectedAssetType] = asset.str();

      mAssetWindowOpened = true;
      ImGui::SetWindowFocus("###assetEditWindow");
    }
  }

  if ( mAssetWindowOpened == false )
    return;

  const auto& selectedFontId = mSelectedAssetIds[mSelectedAssetType];

  if ( selectedFontId.empty() == true )
    return;

  const auto windowTitle = format("Font '{}'###assetEditWindow",
                                  mSelectedAssetIds[mSelectedAssetType]);

  if ( ImGui::Begin(windowTitle.c_str(), &mAssetWindowOpened,
                    ImGuiWindowFlags_HorizontalScrollbar) == true )
  {
    if ( fontMgr.status(selectedFontId) != AssetStatus::Undefined )
      fontMgr.load({selectedFontId});

    fontMgr.ui_show_preview(selectedFontId, registry);
  }

  ImGui::End(); // windowTitle
}

void
AssetManagerUi::ui_show_live_meshes(
  entt::registry& registry )
{
  using fmt::format;
  using MeshAssetManager = types::MeshAssetManager;

  auto& meshesMgr = registry.ctx().get <MeshAssetManager> ();
  auto assets = meshesMgr.assetIdList();

  for ( const auto& asset : assets )
  {
    if ( mAssetIdFilter.query(asset.str()) == false )
      continue;

    const bool selected = asset.str() == mSelectedAssetIds[mSelectedAssetType];

    const auto flags =  ImGuiSelectableFlags_SpanAllColumns |
                        ImGuiSelectableFlags_AllowItemOverlap;

    if ( ImGui::Selectable( asset.str().c_str(),
                            selected, flags ) )
    {
      mSelectedAssetIds[mSelectedAssetType] = asset.str();

      mAssetWindowOpened = true;
      ImGui::SetWindowFocus("###assetEditWindow");
    }
  }

  if ( mAssetWindowOpened == false )
    return;

  const auto& selectedMeshId = mSelectedAssetIds[mSelectedAssetType];

  if ( selectedMeshId.empty() == true )
    return;

  const auto windowTitle = format("Mesh '{}'###assetEditWindow",
                                  mSelectedAssetIds[mSelectedAssetType]);

  if ( ImGui::Begin(windowTitle.c_str(), &mAssetWindowOpened,
                    ImGuiWindowFlags_HorizontalScrollbar) == true )
  {
    if ( meshesMgr.status(selectedMeshId) != AssetStatus::Undefined )
      meshesMgr.load({selectedMeshId});

    meshesMgr.ui_show_preview(selectedMeshId, registry);
  }

  ImGui::End(); // windowTitle
}

void
AssetManagerUi::ui_show_live_mouse_cursors(
  entt::registry& registry )
{
  using fmt::format;
  using MouseCursorAssetManager = types::MouseCursorAssetManager;

  auto& mouseCursorMgr = registry.ctx().get <MouseCursorAssetManager> ();
  auto assets = mouseCursorMgr.assetIdList();

  for ( const auto& asset : assets )
  {
    if ( mAssetIdFilter.query(asset.str()) == false )
      continue;

    const bool selected = asset.str() == mSelectedAssetIds[mSelectedAssetType];

    const auto flags =  ImGuiSelectableFlags_SpanAllColumns |
                        ImGuiSelectableFlags_AllowItemOverlap;

    if ( ImGui::Selectable( asset.str().c_str(),
                            selected, flags ) )
    {
      mSelectedAssetIds[mSelectedAssetType] = asset.str();

      mAssetWindowOpened = true;
      ImGui::SetWindowFocus("###assetEditWindow");
    }
  }

  if ( mAssetWindowOpened == false )
    return;

  const auto& selectedMouseCursorId = mSelectedAssetIds[mSelectedAssetType];

  if ( selectedMouseCursorId.empty() == true )
    return;

  const auto windowTitle = format("Mouse cursor '{}'###assetEditWindow",
                                  mSelectedAssetIds[mSelectedAssetType]);

  if ( ImGui::Begin(windowTitle.c_str(), &mAssetWindowOpened,
                    ImGuiWindowFlags_HorizontalScrollbar) == true )
  {
    if ( mouseCursorMgr.status(selectedMouseCursorId) != AssetStatus::Undefined )
      mouseCursorMgr.load({selectedMouseCursorId});

    mouseCursorMgr.ui_show_preview(selectedMouseCursorId, registry);
  }

  ImGui::End(); // windowTitle
}

void
AssetManagerUi::ui_show_live_terrain(
  entt::registry& registry )
{
  using fmt::format;
  using TerrainAssetManager = types::TerrainAssetManager;

  auto& terrainMgr = registry.ctx().get <TerrainAssetManager> ();
  auto assets = terrainMgr.assetIdList();

  for ( const auto& asset : assets )
  {
    if ( mAssetIdFilter.query(asset.str()) == false )
      continue;

    const bool selected = asset.str() == mSelectedAssetIds[mSelectedAssetType];

    const auto flags =  ImGuiSelectableFlags_SpanAllColumns |
                        ImGuiSelectableFlags_AllowItemOverlap;

    if ( ImGui::Selectable( asset.str().c_str(),
                            selected, flags ) )
    {
      mSelectedAssetIds[mSelectedAssetType] = asset.str();

      mAssetWindowOpened = true;
      ImGui::SetWindowFocus("###assetEditWindow");
    }
  }

  if ( mAssetWindowOpened == false )
    return;

  const auto& selectedTerrainId = mSelectedAssetIds[mSelectedAssetType];

  if ( selectedTerrainId.empty() == true )
    return;

  const auto windowTitle = format("Terrain '{}'###assetEditWindow",
                                  mSelectedAssetIds[mSelectedAssetType]);

  if ( ImGui::Begin(windowTitle.c_str(), &mAssetWindowOpened,
                    ImGuiWindowFlags_HorizontalScrollbar) == true )
  {
    if ( terrainMgr.status(selectedTerrainId) != AssetStatus::Undefined )
      terrainMgr.load({selectedTerrainId});

    terrainMgr.ui_show_preview(selectedTerrainId, registry);
  }

  ImGui::End(); // windowTitle
}

void
AssetManagerUi::ui_show_live_text(
  entt::registry& registry )
{
  using fmt::format;
  using TextStringAssetManager = types::TextStringAssetManager;

  auto& textMgr = registry.ctx().get <TextStringAssetManager> ();
  auto assets = textMgr.assetIdList();

  for ( const auto& asset : assets )
  {
    if ( mAssetIdFilter.query(asset.str()) == false )
      continue;

    const bool selected = asset.str() == mSelectedAssetIds[mSelectedAssetType];

    const auto flags =  ImGuiSelectableFlags_SpanAllColumns |
                        ImGuiSelectableFlags_AllowItemOverlap;

    if ( ImGui::Selectable( asset.str().c_str(),
                            selected, flags ) )
    {
      mSelectedAssetIds[mSelectedAssetType] = asset.str();

      mAssetWindowOpened = true;
      ImGui::SetWindowFocus("###assetEditWindow");
    }
  }

  if ( mAssetWindowOpened == false )
    return;

  const auto& selectedTextId = mSelectedAssetIds[mSelectedAssetType];

  if ( selectedTextId.empty() == true )
    return;

  const auto windowTitle = format("Text '{}'###assetEditWindow",
                                  mSelectedAssetIds[mSelectedAssetType]);

  if ( ImGui::Begin(windowTitle.c_str(), &mAssetWindowOpened,
                    ImGuiWindowFlags_HorizontalScrollbar) == true )
  {
    if ( textMgr.status(selectedTextId) != AssetStatus::Undefined )
      textMgr.load({selectedTextId});

    textMgr.ui_show_preview(selectedTextId, registry);
  }

  ImGui::End(); // windowTitle
}

void
AssetManagerUi::ui_show_live_texture(
  entt::registry& registry )
{
  using fmt::format;
  using TextureAssetManager = types::TextureAssetManager;

  auto& textureMgr = registry.ctx().get <TextureAssetManager> ();
  auto assets = textureMgr.assetIdList();

  for ( const auto& asset : assets )
  {
    if ( mAssetIdFilter.query(asset.str()) == false )
      continue;

    const bool selected = asset.str() == mSelectedAssetIds[mSelectedAssetType];

    const auto flags =  ImGuiSelectableFlags_SpanAllColumns |
                        ImGuiSelectableFlags_AllowItemOverlap;

    if ( ImGui::Selectable( asset.str().c_str(),
                            selected, flags ) )
    {
      mSelectedAssetIds[mSelectedAssetType] = asset.str();

      mAssetWindowOpened = true;
      ImGui::SetWindowFocus("###assetEditWindow");
    }
  }

  if ( mAssetWindowOpened == false )
    return;

  const auto& selectedTextureId = mSelectedAssetIds[mSelectedAssetType];

  if ( selectedTextureId.empty() == true )
    return;

  const auto windowTitle = format("Texture '{}'###assetEditWindow", selectedTextureId);

  if ( ImGui::Begin(windowTitle.c_str(), &mAssetWindowOpened,
                    ImGuiWindowFlags_HorizontalScrollbar) == true )
  {
    if ( textureMgr.status(selectedTextureId) != AssetStatus::Undefined )
      textureMgr.load({selectedTextureId});

    textureMgr.ui_show_preview(selectedTextureId, registry);
  }

  ImGui::End(); // windowTitle
}

} // namespace cqde::ui
