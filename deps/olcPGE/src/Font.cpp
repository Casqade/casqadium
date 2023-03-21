#include <olcPGE/Font.hpp>

#ifdef WIN32
#include <ft2build.h>
#pragma comment(lib, "freetype.lib")
#else
#include <freetype2/ft2build.h>
#endif

#include <cmath>


namespace olc
{

FT_Library olc::Font::library {};


Font::~Font()
{
    if (fontFace != nullptr)
        FT_Done_Face(fontFace);
}

Font::Font(const std::string& path, const int _fontSize)
  : fontFace(nullptr)
  , fallbacks()
  , fontSize(_fontSize)
{
    LoadFromFile(path, fontSize);
}

Font::Font(Font&& other)
{
    fontFace = other.fontFace;
    fontSize = other.fontSize;
    fallbacks = std::move(other.fallbacks);
    other.fontFace = nullptr;
    other.fallbacks.clear();
}

Font&
Font::operator = (Font&& other)
{
    fontFace = other.fontFace;
    fontSize = other.fontSize;
    fallbacks = std::move(other.fallbacks);
    other.fontFace = nullptr;
    other.fallbacks.clear();
    return *this;
}

olc::rcode
Font::LoadFromFile(
  const std::string& path,
  const int _fontSize )
{
    FT_Error error = FT_New_Face(library, path.c_str(), 0, &fontFace);
    if (error)
    {
        const char *errorString = FT_Error_String(error);
        if (errorString == nullptr)
        {
            errorMessage = "An unknown error occured while creating font face! Error code: ";
            errorMessage += std::to_string(error);
        }
        else
            errorMessage = errorString;

        return olc::rcode::FAIL;
    }

    error = FT_Set_Pixel_Sizes(fontFace, 0, _fontSize);
    if (error)
    {
        const char *errorString = FT_Error_String(error);
        if (errorString == nullptr)
        {
            errorMessage = "An unknown error occured while setting font pixel size! Error code: ";
            errorMessage += std::to_string(error);
        }
        else
            errorMessage = errorString;

        return olc::rcode::FAIL;
    }

    fontSize = _fontSize;

    return olc::rcode::OK;
}

olc::rcode
Font::LoadFromMemory(
  const std::vector <uint32_t>& data,
  const int _fontSize )
{
  const auto fontData = reinterpret_cast <const FT_Byte*> (data.data());

  FT_Error error = FT_New_Memory_Face(library, fontData, data.size() * 4, 0, &fontFace);
  if (error)
  {
      const char *errorString = FT_Error_String(error);
      if (errorString == nullptr)
      {
          errorMessage = "An unknown error occured while creating font face! Error code: ";
          errorMessage += std::to_string(error);
      }
      else
          errorMessage = errorString;

      return olc::rcode::FAIL;
  }

  error = FT_Set_Pixel_Sizes(fontFace, 0, _fontSize);
  if (error)
  {
      const char *errorString = FT_Error_String(error);
      if (errorString == nullptr)
      {
          errorMessage = "An unknown error occured while setting font pixel size! Error code: ";
          errorMessage += std::to_string(error);
      }
      else
          errorMessage = errorString;

      return olc::rcode::FAIL;
  }

  fontSize = _fontSize;

  return olc::rcode::OK;
}

FontRect
Font::GetStringBounds(std::u32string string, float angle)
{
    FT_Matrix rotMat;
    rotMat.xx = (FT_Fixed)(std::cos(angle) * 0x10000L);
    rotMat.xy = (FT_Fixed)(-std::sin(angle) * 0x10000L);
    rotMat.yx = (FT_Fixed)(std::sin(angle) * 0x10000L);
    rotMat.yy = (FT_Fixed)(std::cos(angle) * 0x10000L);

    FT_Vector pen;
    pen.x = 0;
    pen.y = 0;

    olc::FontRect rect;
    int intMax = std::numeric_limits<int>::max();
    int intMin = std::numeric_limits<int>::min();

    int minX = intMax;
    int minY = intMax;
    int maxX = intMin;
    int maxY = intMin;

    Font *prevToUse = nullptr;

    for (size_t i = 0; i < string.size(); i++) {
        char32_t chr = string[i];

        Font *toUse = this;
        FT_UInt chrIndex = GetCharIndex(chr);

        if (chrIndex == 0) {
            for (auto &font : fallbacks) {
                FT_UInt fbChr = font.GetCharIndex(chr);
                if (fbChr != 0) {
                    chrIndex = fbChr;
                    toUse = &font;
                }
            }
        }

        if (prevToUse == toUse) {
            FT_Vector kern;
            FT_Get_Kerning(fontFace, string[i - 1], chr,
                           FT_KERNING_DEFAULT, &kern);

            pen.x += kern.x;
            pen.y += kern.y;
        }

        FT_Set_Transform(toUse->fontFace, &rotMat, &pen);
        FT_Error error = FT_Load_Char(toUse->fontFace, chr,
                                      FT_LOAD_BITMAP_METRICS_ONLY);
        if (error)
        {
            const char *errorString = FT_Error_String(error);
            if (errorString == nullptr)
            {
                errorMessage = "An unknown error occured while loading a glyph! Error code: ";
                errorMessage += std::to_string(error);
            }
            else
                errorMessage = errorString;

            return olc::FontRect{{0, 0}, {0, 0}};
        }

        FT_GlyphSlot slot = toUse->fontFace->glyph;
        FT_Glyph glyph;
        FT_Get_Glyph(slot, &glyph);

        FT_BBox bbox;
        FT_Glyph_Get_CBox(glyph, FT_GLYPH_BBOX_GRIDFIT, &bbox);

        if (bbox.xMin < minX) {
            minX = bbox.xMin;
        }
        if (bbox.xMax > maxX) {
            maxX = bbox.xMax;
        }
        if (bbox.yMin < minY) {
            minY = bbox.yMin;
        }
        if (bbox.yMax > maxY) {
            maxY = bbox.yMax;
        }

        pen.x += slot->advance.x;
        pen.y += slot->advance.y;

        FT_Done_Glyph(glyph);
    }

    minX /= 64;
    minY /= 64;
    maxX /= 64;
    maxY /= 64;
    return olc::FontRect{{minX, -maxY}, {maxX - minX, maxY - minY}};
}

olc::Sprite*
Font::RenderStringToSprite(std::u32string string, olc::Pixel color, const bool antialiased)
{
    olc::FontRect rect = GetStringBounds(string);
    olc::Sprite *sprite = new olc::Sprite{rect.size.x, rect.size.y};

    for (int x = 0; x < rect.size.x; x++) {
        for (int y = 0; y < rect.size.y; y++) {
            sprite->SetPixel(x, y, olc::BLANK);
        }
    }

    FT_Vector pen;
    pen.x = -rect.offset.x;
    pen.y = rect.offset.y * 64;

    olc::Pixel::Mode prevMode = Sprite::nPixelMode;
    Sprite::nPixelMode = olc::Pixel::ALPHA;

    olc::Font *prevToUse = nullptr;

    for (size_t i = 0; i < string.size(); i++) {
        char32_t chr = string[i];

        Font *toUse = this;
        FT_UInt chrIndex = GetCharIndex(chr);

        if (chrIndex == 0) {
            for (auto &font : fallbacks) {
                FT_UInt fbChr = font.GetCharIndex(chr);
                if (fbChr != 0) {
                    chrIndex = fbChr;
                    toUse = &font;
                }
            }
        }

        if (prevToUse == toUse) {
            FT_Vector kern;
            FT_Get_Kerning(fontFace, string[i - 1], chr,
                           FT_KERNING_DEFAULT, &kern);

            pen.x += kern.x;
            pen.y += kern.y;
        }

        int loadFlags = FT_LOAD_RENDER | FT_LOAD_COLOR;
        if (antialiased == false)
          loadFlags |= FT_LOAD_MONOCHROME;

        FT_Set_Transform(toUse->fontFace, nullptr, &pen);
        FT_Error error = FT_Load_Char(toUse->fontFace, chr,
                                      loadFlags);
        if (error)
        {
            delete sprite;

            const char *errorString = FT_Error_String(error);
            if (errorString == nullptr)
            {
                errorMessage = "An unknown error occured while rendering a glyph! Error code: ";
                errorMessage += std::to_string(error);
            }
            else
                errorMessage = errorString;

            return nullptr;
        }

        FT_Bitmap bmp = toUse->fontFace->glyph->bitmap;
        FT_GlyphSlot slot = toUse->fontFace->glyph;
        DrawBitmapTo(slot->bitmap_left,
                     slot->bitmap_top, bmp, color,
                     sprite);

        pen.x += toUse->fontFace->glyph->advance.x;
        pen.y += toUse->fontFace->glyph->advance.y;
    }

    Sprite::nPixelMode = prevMode;

    return sprite;
}

olc::Decal*
Font::RenderStringToDecal(std::u32string string, olc::Pixel color, const bool antialiased)
{
    Sprite *sprite = RenderStringToSprite(string, color, antialiased);
    olc::Decal *decal = new olc::Decal{sprite};
    return decal;
}

olc::Renderable
Font::RenderStringToRenderable(std::u32string string, olc::Pixel color, const bool antialiased)
{
    Sprite *sprite = RenderStringToSprite(string, color, antialiased);
    olc::Renderable renderable;
    renderable.Create(sprite->width, sprite->height);

    for (int x = 0; x < sprite->width; x++) {
        for (int y = 0; y < sprite->height; y++) {
            renderable.Sprite()->SetPixel(x, y, sprite->GetPixel(x, y));
        }
    }

    delete sprite;

    renderable.Decal()->Update();

    return renderable;
}

void
Font::AddFallbackFont(std::string path)
{
    fallbacks.emplace_back(path, fontSize);
}

int
Font::GetFontSize() const
{
  return fontSize;
}

std::string
Font::GetErrorMessage() const
{
  return errorMessage;
}

std::string
Font::Init()
{
    FT_Error error = FT_Init_FreeType(&library);

    if (error)
    {

        const char* errorString = FT_Error_String(error);

        if (errorString != nullptr)
          return errorString;

        return
          "An unknown error occured while loading the font library! "
          "Error code: " + std::to_string(error);
    }

    return {};
}

std::string
Font::Deinit()
{
    FT_Error error = FT_Done_FreeType(library);

    if (error)
    {
        std::string ftErrorMessage;

        const char* errorString = FT_Error_String(error);

        if (errorString != nullptr)
          return errorString;

        return
          "An unknown error occured while unloading the font library! "
          "Error code: " + std::to_string(error);
    }

    return {};
}

void
Font::DrawBitmapTo(int x, int y, FT_Bitmap bmp,
                   olc::Pixel color, olc::Sprite* sprite)
{
    switch (bmp.pixel_mode) {
    case FT_PIXEL_MODE_MONO:
        for (size_t bx = 0; bx < bmp.width; bx++) {
            for (size_t by = 0; by < bmp.rows; by++) {
                int byteOffset = bx / 8;
                char byte = bmp.buffer[by * bmp.pitch + byteOffset];
                bool val = (byte >> (7 - bx % 8)) & 1;
                if (val) {
                    sprite->SetPixel(x + bx, y + by, color);
                }
            }
        }
        break;
    case FT_PIXEL_MODE_GRAY:
        for (size_t bx = 0; bx < bmp.width; bx++) {
            for (size_t by = 0; by < bmp.rows; by++) {
                uint8_t byte = bmp.buffer[by * bmp.pitch + bx];
                if (byte == 0) {
                    continue;
                }
                color.a = byte;
                sprite->SetPixel(x + bx, y + by, color);
            }
        }
        break;
    case FT_PIXEL_MODE_GRAY2: break;
    case FT_PIXEL_MODE_GRAY4: break;
    case FT_PIXEL_MODE_LCD: break;
    case FT_PIXEL_MODE_LCD_V: break;
    case FT_PIXEL_MODE_BGRA:
        for (size_t bx = 0; bx < bmp.width; bx++) {
            for (size_t by = 0; by < bmp.rows; by++) {
                olc::Pixel pixel{
                        bmp.buffer[by * bmp.pitch + bx * 4 + 2],
                        bmp.buffer[by * bmp.pitch + bx * 4 + 1],
                        bmp.buffer[by * bmp.pitch + bx * 4 + 0],
                        bmp.buffer[by * bmp.pitch + bx * 4 + 3],
                };
                sprite->SetPixel(x + bx, y + by, pixel);
            }
        }
        break;
    }
}

FT_UInt
Font::GetCharIndex(char32_t charCode)
{
    return FT_Get_Char_Index(fontFace, charCode);
}

} // namespace olc