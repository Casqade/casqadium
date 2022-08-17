#pragma once

#include <olcPGE/olcPixelGameEngine.hpp>


namespace olc
{
class Font;
}

namespace cqde
{

std::shared_ptr <olc::Renderable> textureFromText(
  const std::string& text,
  const olc::Pixel&,
  const bool monospaced );

std::shared_ptr <olc::Renderable> textureFromText(
  const std::string& text,
  olc::Font*,
  const olc::Pixel&,
  const bool antialiased );

} // namespace cqde
