#ifndef OLC_PGEX_TTF_H
#define OLC_PGEX_TTF_H

#include <olcPGE/olcPixelGameEngine.hpp>

#ifdef WIN32
#include <ft2build.h>
#pragma comment(lib, "freetype.lib")
#else
#include <freetype2/ft2build.h>
#endif

#include FT_FREETYPE_H
#include FT_GLYPH_H

#include <string>
#include <vector>

namespace olc
{

struct FontRect
{
  olc::vi2d offset;
  olc::vi2d size;
};

class Font : public olc::PGEX
{
public:
  Font() = default;
  ~Font();
  Font(const std::string& path, const int fontSize);

  Font(const Font& other) = delete;
  Font(Font&& other);

  Font& operator=(const Font& other) = delete;
  Font& operator=(Font&& other);

  olc::rcode LoadFromFile(const std::string& path, const int fontSize);

  olc::rcode DrawString(std::u32string string, int x, int y,
                        olc::Pixel color = olc::BLACK,
                        float angle = 0.0f);
  olc::rcode DrawString(std::u32string string, olc::vi2d pos,
                        olc::Pixel color = olc::BLACK,
                        float angle = 0.0f);

  FontRect GetStringBounds(std::u32string string, float angle = 0.0f);

  olc::Sprite* RenderStringToSprite(std::u32string string,
                                    olc::Pixel color);
  olc::Decal* RenderStringToDecal(std::u32string string,
                                  olc::Pixel color);

  olc::Renderable RenderStringToRenderable(std::u32string string,
                                           olc::Pixel color);

  void AddFallbackFont(std::string path);

  std::string GetErrorMessage() const;

  static olc::rcode init();
  static olc::rcode deinit();

private:
  void DrawBitmap(int x, int y, FT_Bitmap bmp, olc::Pixel color);
  void DrawBitmapTo(int x, int y, FT_Bitmap bmp, olc::Pixel color,
                    olc::Sprite* sprite);

  FT_UInt GetCharIndex(char32_t charCode);

  FT_Face fontFace {};
  std::vector<Font> fallbacks {};
  int fontSize {};

  std::string errorMessage {};

  static FT_Library library;
  static std::string libraryErrorMessage;
};

} // namespace olc

#endif
