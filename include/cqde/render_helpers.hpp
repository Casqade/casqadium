#pragma once

#include <olcPGE/Renderable.hpp>

#include <glm/fwd.hpp>


namespace cqde
{

std::shared_ptr <olc::Renderable> textureFromText(
  const std::string& text,
  const olc::Pixel& colorFg,
  const olc::Pixel& colorBg = olc::BLANK,
  const bool monospaced = false );

std::shared_ptr <olc::Renderable> textureFromText(
  const std::string& text,
  olc::Font*,
  const olc::Pixel&,
  const bool antialiased );


olc::Sprite spriteFromText(
  const std::string& text,
  const olc::Pixel& colorFg,
  const olc::Pixel& colorBg = olc::BLANK,
  const bool monospaced = false );

olc::Sprite spriteFromText(
  const std::string& text,
  olc::Font*,
  const olc::Pixel&,
  const bool antialiased );

} // namespace cqde
