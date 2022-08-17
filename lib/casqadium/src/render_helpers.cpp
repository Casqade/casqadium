#pragma once

#include <cqde/render_helpers.hpp>

#include <olcPGE/olcPGEX_TTF.hpp>


namespace cqde
{

std::shared_ptr <olc::Renderable>
textureFromText(
  const std::string& text,
  const olc::Pixel& color,
  const bool monospaced )
{
  olc::PixelGameEngine* pge = olc::renderer->ptrPGE;

  const auto textSize = monospaced ?
                          pge->GetTextSize(text)
                        : pge->GetTextSizeProp(text);

  auto texture = std::make_shared <olc::Renderable> ();
  texture->Create(textSize.x, textSize.y);

  const auto drawTargetPrev = pge->GetDrawTarget();

  pge->SetDrawTarget(texture->Sprite());

  monospaced ?
      pge->DrawString({0, 0}, text, color)
    : pge->DrawStringProp({0, 0}, text, color);

  pge->SetDrawTarget(drawTargetPrev);

  texture->Decal()->Update();

  return texture;
}

std::shared_ptr <olc::Renderable>
textureFromText(
  const std::string& text,
  olc::Font* font,
  const olc::Pixel& color,
  const bool antialiased )
{
  const std::u32string textU32 {text.begin(), text.end()};
  auto renderable = font->RenderStringToRenderable(textU32, color, antialiased);

  return std::make_shared <olc::Renderable> (std::move(renderable));
}

} // namespace cqde
