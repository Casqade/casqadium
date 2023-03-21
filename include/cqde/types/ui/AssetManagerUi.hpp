#pragma once

#include <cqde/alias.hpp>

#include <cqde/types/assets/AssetStatus.hpp>

#include <cqde/types/ui/widgets/PackageFilter.hpp>
#include <cqde/types/ui/widgets/StringFilter.hpp>

#include <cqde/types/UndoRedoQueue_types.hpp>
#include <cqde/types/Package.hpp>

#include <entt/fwd.hpp>


namespace cqde::ui
{

class AssetManagerUi
{
  using AssetStatus = types::AssetStatus;
  using AssetsState = std::map <PackageId, Json::Value>;
  using UndoRedoQueue = types::UndoRedoQueue <AssetsState>;
  using ContentType = types::Package::ContentType;

  PackageFilter mPackageFilter {};
  StringFilter  mAssetIdFilter {"Asset ID"};

  std::map <ContentType, std::string> mAssetTypeNames {};

  std::map <ContentType, std::string> mSelectedAssetIds {};
  ContentType mSelectedAssetType {};

  std::string mNewAssetName {"NewAsset"};
  std::string mRenamedAssetId {};

  bool mAssetWindowOpened {};

  AssetsState mAssetsState {};

  UndoRedoQueue mHistoryBuffer {32};

public:
  AssetManagerUi();

  void ui_show( entt::registry& );

private:
  void stateApply( entt::registry& );
  void stateSave( const PackageId&, entt::registry& );

  void ui_show_live_state( entt::registry& );
  void ui_show_package_state( entt::registry& );

  void ui_show_menu_bar( entt::registry& );
  void ui_show_asset_window( entt::registry& );

  void ui_show_live_audio( entt::registry& );
  void ui_show_live_font( entt::registry& );
  void ui_show_live_geometry( entt::registry& );
  void ui_show_live_mouse_cursors( entt::registry& );
  void ui_show_live_terrain( entt::registry& );
  void ui_show_live_text( entt::registry& );
  void ui_show_live_texture( entt::registry& );
};

} // namespace cqde::ui
