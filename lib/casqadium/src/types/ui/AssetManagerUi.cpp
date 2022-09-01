#include <cqde/types/ui/AssetManagerUi.hpp>
#include <cqde/types/PackageManager.hpp>

#include <cqde/types/assets/FontAssetManager.hpp>
#include <cqde/types/assets/GeometryAssetManager.hpp>
#include <cqde/types/assets/TextStringAssetManager.hpp>
#include <cqde/types/assets/TextureAssetManager.hpp>

#include <cqde/common.hpp>
#include <cqde/file_helpers.hpp>
#include <cqde/util/logger.hpp>

#include <entt/entity/registry.hpp>

#include <imgui.h>
#include <imgui_stdlib.h>


namespace cqde::ui
{

AssetManagerUi::AssetManagerUi()
{
  mAssetTypeNames =
  {
    {ContentType::Fonts, "Fonts"},
    {ContentType::Geometry, "Geometry"},
    {ContentType::Text, "Text"},
    {ContentType::Textures, "Textures"},
  };

  mSelectedAssetIds =
  {
    {ContentType::Fonts, {}},
    {ContentType::Geometry, {}},
    {ContentType::Text, {}},
    {ContentType::Textures, {}},
  };

  mSelectedAssetType = mAssetTypeNames.begin()->first;
}

void
AssetManagerUi::stateApply(
  entt::registry& registry )
{
  using FontAssetManager = types::FontAssetManager;
  using GeometryAssetManager = types::GeometryAssetManager;
  using TextStringAssetManager = types::TextStringAssetManager;
  using TextureAssetManager = types::TextureAssetManager;

  using PackageManager = types::PackageManager;
  using ContentType = types::Package::ContentType;

  auto& fontMgr = registry.ctx().at <FontAssetManager> ();
  auto& geometryMgr = registry.ctx().at <GeometryAssetManager> ();
  auto& textMgr = registry.ctx().at <TextStringAssetManager> ();
  auto& textureMgr = registry.ctx().at <TextureAssetManager> ();

  mAssetsState.clear();

  fontMgr.clear();
  geometryMgr.clear();
  textMgr.clear();
  textureMgr.clear();

  const auto& pkgMgr = registry.ctx().at <PackageManager> ();

  for ( const auto& packageId : pkgMgr.packages() )
  {
    auto package = pkgMgr.package(packageId);
    CQDE_ASSERT_DEBUG(package != nullptr, continue);

    const auto fontsPath = package->contentPath(ContentType::Fonts);
    const auto geometryPath = package->contentPath(ContentType::Geometry);
    const auto textPath = package->contentPath(ContentType::Text);
    const auto texturesPath = package->contentPath(ContentType::Textures);

    fontMgr.parseAssetDbFile(fontsPath);
    geometryMgr.parseAssetDbFile(geometryPath);
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

  auto& pkgMgr = registry.ctx().at <PackageManager> ();

  auto package = pkgMgr.package(packageId);

  if ( package == nullptr )
  {
    LOG_ERROR("Failed to write package '{}' input config: "
              "No such package in PackageManager",
              packageId.str());
    return;
  }

  const auto& fontsDb = mAssetsState[packageId][mAssetTypeNames.at(ContentType::Fonts)];
  const auto& geometryDb = mAssetsState[packageId][mAssetTypeNames.at(ContentType::Geometry)];
  const auto& textDb = mAssetsState[packageId][mAssetTypeNames.at(ContentType::Text)];
  const auto& texturesDb = mAssetsState[packageId][mAssetTypeNames.at(ContentType::Textures)];

  if ( fontsDb.isNull() == false )
    package->save(ContentType::Fonts, fontsDb);

  if ( geometryDb.isNull() == false )
    package->save(ContentType::Geometry, geometryDb);

  if ( textDb.isNull() == false )
    package->save(ContentType::Text, textDb);

  if ( texturesDb.isNull() == false )
    package->save(ContentType::Textures, texturesDb);
}

void
AssetManagerUi::ui_show(
  entt::registry& registry )
{
  using FontAssetManager = types::FontAssetManager;
  using GeometryAssetManager = types::GeometryAssetManager;
  using TextStringAssetManager = types::TextStringAssetManager;
  using TextureAssetManager = types::TextureAssetManager;

  using PackageManager = types::PackageManager;
  using ContentType = types::Package::ContentType;

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

  auto& pkgMgr = registry.ctx().at <PackageManager> ();

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
    case ContentType::Fonts:
    {
      ui_show_live_font(registry);
      break;
    }
    case ContentType::Geometry:
    {
      ui_show_live_geometry(registry);
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

  using FontAssetManager = types::FontAssetManager;
  using GeometryAssetManager = types::GeometryAssetManager;
  using TextStringAssetManager = types::TextStringAssetManager;
  using TextureAssetManager = types::TextureAssetManager;

  const auto assetTypeName = mAssetTypeNames.at(mSelectedAssetType);

  auto& assetDb = mAssetsState[mPackageFilter.package()][assetTypeName];

  if ( assetDb.isNull() == true )
  {
    const auto& pkgMgr = registry.ctx().at <PackageManager> ();

    auto package = pkgMgr.package(mPackageFilter.package());

    CQDE_ASSERT_DEBUG(package != nullptr, return);

    const auto assetDbPath = package->contentPath(mSelectedAssetType);

    if ( fileExists(assetDbPath) == false )
      return;

    assetDb = fileParse(assetDbPath);

    auto configIter = mHistoryBuffer.current();
    if ( mHistoryBuffer.isValid(configIter) == true )
      (*configIter)[mPackageFilter.package()][assetTypeName] = assetDb;
  }

  switch (mSelectedAssetType)
  {
    case ContentType::Fonts:
    {
      FontAssetManager::Validate(assetDb);
      break;
    }
    case ContentType::Geometry:
    {
      GeometryAssetManager::Validate(assetDb);
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

  ImGui::BeginDisabled(assetDb.isMember(mNewAssetName) == true);

  const bool newAssetInserted = ImGui::Button("+##assetAdd");

  ImGui::EndDisabled();

  ImGui::SameLine();
  ImGui::InputTextWithHint("##newAssetId", "New asset ID", &mNewAssetName,
                           ImGuiInputTextFlags_AutoSelectAll);

  if ( newAssetInserted == true &&
       mNewAssetName.empty() == false )
  {
    Json::Value newAsset {};

    switch (mSelectedAssetType)
    {
      case ContentType::Fonts:
        newAsset = FontAssetManager::AssetJsonDbEntryReference();
        break;

      case ContentType::Geometry:
        newAsset = GeometryAssetManager::AssetJsonDbEntryReference();
        break;

      case ContentType::Text:
        newAsset = TextStringAssetManager::AssetJsonDbEntryReference();
        break;

      case ContentType::Textures:
        newAsset = TextureAssetManager::AssetJsonDbEntryReference();
        break;

      defaut:
        CQDE_ASSERT_DEBUG(false, newAsset = Json::objectValue);
        break;
    }

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
        stateSave(packageId, registry);

    if ( ImGui::MenuItem("Save & apply") )
    {
      for ( const auto& [packageId, state] : mAssetsState )
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

  using FontAssetManager = types::FontAssetManager;
  using GeometryAssetManager = types::GeometryAssetManager;
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
    case ContentType::Fonts:
    {
      registry.ctx().at <FontAssetManager> ().ui_show(assetEntry);
      break;
    }
    case ContentType::Geometry:
    {
      registry.ctx().at <GeometryAssetManager> ().ui_show(assetEntry);
      break;
    }
    case ContentType::Text:
    {
      registry.ctx().at <TextStringAssetManager> ().ui_show(assetEntry);
      break;
    }
    case ContentType::Textures:
    {
      registry.ctx().at <TextureAssetManager> ().ui_show(assetEntry);
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
AssetManagerUi::ui_show_live_font(
  entt::registry& registry )
{
  using fmt::format;
  using FontAssetManager = types::FontAssetManager;

  auto& fontMgr = registry.ctx().at <FontAssetManager> ();
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

    fontMgr.ui_show_preview(selectedFontId);
  }

  ImGui::End(); // windowTitle
}

void
AssetManagerUi::ui_show_live_geometry(
  entt::registry& registry )
{
  using fmt::format;
  using GeometryAssetManager = types::GeometryAssetManager;

  auto& geometryMgr = registry.ctx().at <GeometryAssetManager> ();
  auto assets = geometryMgr.assetIdList();

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

  const auto& selectedGeometryId = mSelectedAssetIds[mSelectedAssetType];

  if ( selectedGeometryId.empty() == true )
    return;

  const auto windowTitle = format("Geometry '{}'###assetEditWindow",
                                  mSelectedAssetIds[mSelectedAssetType]);

  if ( ImGui::Begin(windowTitle.c_str(), &mAssetWindowOpened,
                    ImGuiWindowFlags_HorizontalScrollbar) == true )
  {
    if ( geometryMgr.status(selectedGeometryId) != AssetStatus::Undefined )
      geometryMgr.load({selectedGeometryId});

    geometryMgr.ui_show_preview(selectedGeometryId);
  }

  ImGui::End(); // windowTitle
}

void
AssetManagerUi::ui_show_live_text(
  entt::registry& registry )
{
  using fmt::format;
  using TextStringAssetManager = types::TextStringAssetManager;

  auto& textMgr = registry.ctx().at <TextStringAssetManager> ();
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

    textMgr.ui_show_preview(selectedTextId);
  }

  ImGui::End(); // windowTitle
}

void
AssetManagerUi::ui_show_live_texture(
  entt::registry& registry )
{
  using fmt::format;
  using TextureAssetManager = types::TextureAssetManager;

  auto& textureMgr = registry.ctx().at <TextureAssetManager> ();
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

    textureMgr.ui_show_preview(selectedTextureId);
  }

  ImGui::End(); // windowTitle
}

} // namespace cqde::ui
