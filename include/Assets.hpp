#ifndef ASSETS_HPP
#define ASSETS_HPP

#include <map>
#include <string>
#include <memory>


template <class Asset, typename AssetId>
class Assets
{
protected:
  static std::string AssetsPath;
  static std::string AssetType;

  static std::map <AssetId, std::shared_ptr <Asset>> mAssets;

public:
  static bool Load( const AssetId, const std::string& );
  static Asset& Get( const AssetId );
};


#endif
