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

class OLCPGE_EXPORT Font : public olc::PGEX
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
  olc::rcode LoadFontDefault(const int fontSize);

  olc::rcode DrawString(std::u32string string, int x, int y,
                        olc::Pixel color = olc::BLACK,
                        float angle = 0.0f,
                        const bool antialiased = true);
  olc::rcode DrawString(std::u32string string, olc::vi2d pos,
                        olc::Pixel color = olc::BLACK,
                        float angle = 0.0f,
                        const bool antialiased = true);

  FontRect GetStringBounds(std::u32string string, float angle = 0.0f);

  olc::Sprite* RenderStringToSprite(std::u32string string,
                                    olc::Pixel color,
                                    const bool antialiased = true);
  olc::Decal* RenderStringToDecal(std::u32string string,
                                  olc::Pixel color,
                                  const bool antialiased = true);

  olc::Renderable RenderStringToRenderable(std::u32string string,
                                           olc::Pixel color,
                                           const bool antialiased = true);

  void AddFallbackFont(std::string path);

  int GetFontSize() const;
  std::string GetErrorMessage() const;

  static std::string GetLibraryErrorMessage();

  static olc::rcode OLCPGE_EXPORT init();
  static olc::rcode OLCPGE_EXPORT deinit();

private:
  void DrawBitmap(int x, int y, FT_Bitmap bmp, olc::Pixel color);
  void DrawBitmapTo(int x, int y, FT_Bitmap bmp, olc::Pixel color,
                    olc::Sprite* sprite);

  FT_UInt GetCharIndex(char32_t charCode);

  FT_Face fontFace {};
  std::vector<Font> fallbacks {};
  int fontSize {};

  std::string errorMessage {};

  static FT_Library OLCPGE_EXPORT library;
  static std::string OLCPGE_EXPORT libraryErrorMessage;

  static std::vector <uint32_t> FontDefault;
};

} // namespace olc

#endif
