#include <SFML/Graphics/Font.hpp>
#include <SFML/Audio/SoundBuffer.hpp>
#include <SFML/Audio/Music.hpp>

#include <Assets.hpp>
#include <Variables.hpp>


template <class Asset, typename AssetId>
std::map <AssetId, std::shared_ptr <Asset>>
Assets <Asset, AssetId>::mAssets = {};


template <class Asset, typename AssetName>
std::string Assets <Asset, AssetName>::AssetsPath = "assets/";

template <>
std::string Assets <sf::Font, FontId>::AssetType = "fonts/";

template <>
std::string Assets <sf::Texture, TextureId>::AssetType = "textures/";

template <>
std::string Assets <sf::SoundBuffer, SoundId>::AssetType = "sounds/";

template <>
std::string Assets <sf::Music, MusicId>::AssetType = "music/";

bool
Music::Load(
  const MusicId       assetId,
  const std::string&  assetPath )
{
  if ( assetPath.empty() )
    return true;

  std::shared_ptr <sf::Music> music = std::make_shared <sf::Music> ();
  if ( music->openFromFile( AssetsPath + AssetType + assetPath ) == true )
  {
    mAssets[assetId] = music;
    return true;
  }

  return false;
}

sf::Music&
Music::Get( const MusicId id )
{
  return Assets::Get(id);
}

template <class Asset, typename AssetId>
bool
Assets <Asset, AssetId>::Load(
  const AssetId       assetId,
  const std::string&  assetPath )
{
  if ( assetPath.empty() )
    return true;

  bool result = false;
  std::shared_ptr <Asset> asset = std::make_shared <Asset> ();

  result = asset->loadFromFile( AssetsPath + AssetType + assetPath );

  if ( result == true )
  {
    mAssets[assetId] = asset;
    return true;
  }

  return false;
}

template <class Asset, typename AssetId>
Asset&
Assets <Asset, AssetId>::Get( const AssetId assetId )
{
  return *(mAssets.at( assetId ));
}

template class
Assets <sf::Font, FontId>;

template class
Assets <sf::Texture, TextureId>;

template class
Assets <sf::SoundBuffer, SoundId>;
