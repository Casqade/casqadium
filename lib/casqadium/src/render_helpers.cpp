#include <cqde/render_helpers.hpp>

#include <olcPGE/olcPGEX_TTF.hpp>


namespace cqde
{

void drawLines(
  const std::vector <olc::vf2d>& vertices,
  const olc::Pixel& color,
  const LineRenderMode mode )
{
  auto pge = olc::renderer->ptrPGE;

  switch (mode)
  {
    case LineRenderMode::Strip:
    {
      for ( size_t i = 0, iNext = 1;
            iNext < vertices.size();
            ++i, ++iNext )
        pge->DrawLineDecal( vertices[i],
                            vertices[iNext],
                            color );
      return;
    }

    case LineRenderMode::Loop:
    {
      for ( size_t i = 0, iNext = 1;
            i < vertices.size();
            ++i, iNext = (i + 1) % vertices.size() )
        pge->DrawLineDecal( vertices[i],
                            vertices[iNext],
                            color );
      return;
    }
  }
}

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
