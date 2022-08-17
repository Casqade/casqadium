#pragma once

#include <olcPGE/olcPixelGameEngine.hpp>


namespace cqde
{

std::shared_ptr <olc::Renderable> textureFromText(
  const std::string& text,
  const olc::Pixel&,
  const bool monospaced );

} // namespace cqde
