#pragma once

#include <cqde/render_helpers.hpp>


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

} // namespace cqde
