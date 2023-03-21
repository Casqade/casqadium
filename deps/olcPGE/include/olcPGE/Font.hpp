#pragma once

#include <olcPGE/Renderable.hpp>

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

const std::vector <uint32_t>& OLCPGE_EXPORT FontProggyCleanData();
const std::vector <uint32_t>& OLCPGE_EXPORT FontRobotoMediumData();

struct FontRect
{
  olc::vi2d offset;
  olc::vi2d size;
};

///
/// olcPGEX_TTF
///
class OLCPGE_EXPORT Font
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
  olc::rcode LoadFromMemory(const std::vector <uint32_t>& fontData, const int fontSize);

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

  static std::string OLCPGE_EXPORT Init();
  static std::string OLCPGE_EXPORT Deinit();

private:
  void DrawBitmapTo(int x, int y, FT_Bitmap bmp, olc::Pixel color,
                    olc::Sprite* sprite);

  FT_UInt GetCharIndex(char32_t charCode);

  FT_Face fontFace {};
  std::vector<Font> fallbacks {};
  int fontSize {};

  std::string errorMessage {};

  static FT_Library OLCPGE_EXPORT library;
};

} // namespace olc