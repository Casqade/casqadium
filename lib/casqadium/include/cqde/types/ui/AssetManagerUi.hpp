#pragma once

#include <cqde/alias.hpp>

#include <cqde/types/ui/widgets/PackageFilter.hpp>
#include <cqde/types/Package.hpp>

#include <entt/fwd.hpp>


namespace cqde::ui
{

class AssetManagerUi
{
  using ContentType = types::Package::ContentType;

  PackageFilter mPackageFilter {};

  std::map <ContentType, std::string> mAssetTypeNames {};
  ContentType mSelectedAssetType {};

public:
  AssetManagerUi();

  void ui_show( entt::registry& );
};

} // namespace cqde::ui
