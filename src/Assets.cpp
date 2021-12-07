#include <SFML/Audio/SoundBuffer.hpp>
#include <SFML/Audio/Music.hpp>

#include <Assets.hpp>
#include <Variables.hpp>

#include <olcPGE/olcPixelGameEngine.hpp>
#include <olcPGE/olcPGEX_CustomFont.hpp>


template <class Asset, typename AssetId>
std::map <AssetId, std::shared_ptr <Asset>>
Assets <Asset, AssetId>::mAssets = {};


template <class Asset, typename AssetName>
std::string Assets <Asset, AssetName>::AssetsPath = "assets/";

template <>
std::string Assets <olc::CustomFont, FontId>::AssetType = "fonts/";

template <>
std::string Assets <olc::Sprite, SpriteId>::AssetType = "sprites/";

template <>
std::string Assets <sf::SoundBuffer, SoundId>::AssetType = "sounds/";

template <>
std::string Assets <sf::Music, MusicId>::AssetType = "music/";

template <class Asset, typename AssetId>
Asset&
Assets <Asset, AssetId>::Get( const AssetId assetId )
{
  return *(mAssets.at( assetId ));
}

template <class Asset, typename AssetId>
bool
Assets <Asset, AssetId>::Load(
  const AssetId       assetId,
  const std::string&  assetPath )
{
//  abstract implementation
  return false;
}

template <>
bool
Assets <std::string, StringId>::Load(
  const StringId      stringId,
  const std::string&  stringValue )
{
  mAssets[stringId] = std::make_shared <std::string> (stringValue);
  return true;
}

template <>
bool
Assets <olc::CustomFont, FontId>::Load(
  const FontId        assetId,
  const std::string&  assetPath )
{
  if ( assetPath.empty() )
    return true;

  auto font = std::make_shared <olc::CustomFont> ();

  if ( font->LoadFromFile( AssetsPath + AssetType + assetPath ) == false )
    return false;

  mAssets[assetId] = font;
  return true;
}

template <>
bool
Assets <olc::Sprite, SpriteId>::Load(
  const SpriteId      assetId,
  const std::string&  assetPath )
{
  if ( assetPath.empty() )
    return true;

  auto sprite = std::make_shared <olc::Sprite> ();

  if ( sprite->LoadFromFile( AssetsPath + AssetType + assetPath ) == false )
    return false;

  mAssets[assetId] = sprite;
  return true;
}

template <>
bool
Assets <sf::SoundBuffer, SoundId>::Load(
  const SoundId       assetId,
  const std::string&  assetPath )
{
  if ( assetPath.empty() )
    return true;

  auto sound = std::make_shared <sf::SoundBuffer> ();
  if ( sound->loadFromFile( AssetsPath + AssetType + assetPath ) == false )
    return false;

  mAssets[assetId] = sound;
  return true;
}

template <>
bool
Assets <sf::Music, MusicId>::Load(
  const MusicId       assetId,
  const std::string&  assetPath )
{
  if ( assetPath.empty() )
    return true;

  auto music = std::make_shared <sf::Music> ();
  if ( music->openFromFile( AssetsPath + AssetType + assetPath ) == false )
    return false;

  mAssets[assetId] = music;
  return true;
}

template class
Assets <std::string, StringId>;

template class
Assets <olc::CustomFont, FontId>;

template class
Assets <olc::Sprite, SpriteId>;

template class
Assets <sf::SoundBuffer, SoundId>;

template class
Assets <sf::Music, MusicId>;
