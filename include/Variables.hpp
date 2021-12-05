#ifndef VARIABLES_HPP
#define VARIABLES_HPP

#include <Assets.hpp>


namespace sf
{
class SoundBuffer;
class Music;
}

namespace olc
{
class CustomFont;
class Sprite;
}

enum class StringId : uint32_t
{
  Foreword,
  ForewordInputPrompt,

  Title,
  Backstory,
};

enum class FontId : uint32_t
{
  FranklinGothic,
  FranklinGothicItalic,
  Munro,
  Jetbrains,
};

enum class TextureId : uint32_t
{
  WindowIcon,
};

enum class SoundId : uint32_t
{
  Null,
};

enum class MusicId : uint32_t
{
  TitleTheme,
};

extern class Logger Log;

class Strings : public Assets <std::string, StringId>
{
public:
  static bool Load( const StringId, const std::string& );
  static std::string& Get( const StringId );
};

class Fonts : public Assets <olc::CustomFont, FontId>
{};

class Textures : public Assets <olc::Sprite, TextureId>
{};

class Sounds : public Assets <sf::SoundBuffer, SoundId>
{};

class Music : public Assets <sf::Music, MusicId>
{
public:
  static bool Load( const MusicId, const std::string& );
  static sf::Music& Get( const MusicId );
};

//extern template class
//Assets <sf::Music, MusicId>;

//extern template
//std::string
//Assets <sf::Music, MusicId>::AssetType;


#endif
