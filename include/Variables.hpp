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
class Renderable;
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
  FranklinGothic30,
  FranklinGothic40,
  FranklinGothic64,
  FranklinGothic128,
  FranklinGothicItalic,
  Munro,
  Jetbrains,
};

enum class SpriteId : uint32_t
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
{};

class Fonts : public Assets <olc::CustomFont, FontId>
{};

class Sprites : public Assets <olc::Renderable, SpriteId>
{};

class Sounds : public Assets <sf::SoundBuffer, SoundId>
{};

class Music : public Assets <sf::Music, MusicId>
{};


#endif
