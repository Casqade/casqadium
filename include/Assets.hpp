#ifndef ASSETS_HPP
#define ASSETS_HPP

#include <map>
#include <string>


template <class Asset>
class Assets
{
  static std::string AssetsPath;
  static std::string AssetType;

  static std::map <std::string, Asset> mAssets;

public:
  static bool Load( const std::string&, const std::string& );
  static Asset& Get( const std::string& );
};

template <class Asset>
std::map <std::string, Asset>
Assets <Asset>::mAssets = {};

template <class Asset>
bool
Assets <Asset>::Load( const std::string& assetName,
                      const std::string& assetPath )
{
  Asset asset;
  if ( asset.loadFromFile( AssetsPath + AssetType + assetPath ) == true )
  {
    mAssets[assetName] = asset;
    return true;
  }

  return false;
}

template <class Asset>
Asset&
Assets <Asset>::Get( const std::string& assetName )
{
  return mAssets.at( assetName );
}

#endif
