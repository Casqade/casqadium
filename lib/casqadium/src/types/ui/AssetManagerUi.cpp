#include <cqde/types/ui/AssetManagerUi.hpp>
#include <cqde/types/PackageManager.hpp>

#include <cqde/common.hpp>

#include <entt/entity/registry.hpp>

#include <imgui.h>


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
  mSelectedAssetType = mAssetTypeNames.begin()->first;
}

void
AssetManagerUi::ui_show(
  entt::registry& registry )
{
  using PackageManager = types::PackageManager;
  using ContentType = types::Package::ContentType;

  if ( ImGui::Begin("Assets", nullptr,
                    ImGuiWindowFlags_HorizontalScrollbar ) == false )
  {
    ImGui::End(); // Assets
    return;
  }

  ImGui::AlignTextToFramePadding();
  ImGui::Text("Package:");

  ImGui::SameLine();
  mPackageFilter.select(registry);

  auto& pkgMgr = registry.ctx().at <PackageManager> ();

  auto package = pkgMgr.package(mPackageFilter.package());

  if ( package == nullptr )
    return ImGui::End(); // Assets

  if ( ImGui::BeginCombo("##assetTypeCombo", mAssetTypeNames.at(mSelectedAssetType).c_str() ) )
  {
    for ( const auto& [assetType, assetTypeName] : mAssetTypeNames )
      if ( ImGui::Selectable(assetTypeName.c_str(), assetType == mSelectedAssetType) )
          mSelectedAssetType = assetType;

    ImGui::EndCombo();
  }

  package->contentPath( mSelectedAssetType );



  ImGui::End(); // Assets
}

} // namespace cqde::ui
