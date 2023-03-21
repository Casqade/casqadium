#pragma once

#include <olcPGE/olcPGE_export.hpp>

#include <cstdint>


namespace olc
{

constexpr uint8_t  nDefaultAlpha = 0xFF;
constexpr uint32_t nDefaultPixel = (nDefaultAlpha << 24);

// O------------------------------------------------------------------------------O
// | olc::Pixel - Represents a 32-Bit RGBA colour                                 |
// O------------------------------------------------------------------------------O
struct OLCPGE_EXPORT Pixel
{
  union
  {
    uint32_t n = nDefaultPixel;
    struct { uint8_t r; uint8_t g; uint8_t b; uint8_t a; };
  };

  enum Mode
  {
    NORMAL, //  No transparency
    MASK,   //  Transparent if alpha is < 255
    ALPHA,  //  Full transparency
  };

  Pixel();
  Pixel(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha = nDefaultAlpha);
  Pixel(uint32_t p);
  Pixel& operator = (const Pixel& v) = default;
  bool   operator ==(const Pixel& p) const;
  bool   operator !=(const Pixel& p) const;
  Pixel  operator * (const float i) const;
  Pixel  operator / (const float i) const;
  Pixel& operator *=(const float i);
  Pixel& operator /=(const float i);
  Pixel  operator + (const Pixel& p) const;
  Pixel  operator - (const Pixel& p) const;
  Pixel& operator +=(const Pixel& p);
  Pixel& operator -=(const Pixel& p);
  Pixel  inv() const;
};

OLCPGE_EXPORT Pixel PixelF(float red, float green, float blue, float alpha = 1.0f);
OLCPGE_EXPORT Pixel PixelLerp(const olc::Pixel& p1, const olc::Pixel& p2, float t);


// O------------------------------------------------------------------------------O
// | USEFUL CONSTANTS                                                             |
// O------------------------------------------------------------------------------O
static const Pixel
GREY(192, 192, 192), DARK_GREY(128, 128, 128), VERY_DARK_GREY(64, 64, 64),
RED(255, 0, 0), DARK_RED(128, 0, 0), VERY_DARK_RED(64, 0, 0),
YELLOW(255, 255, 0), DARK_YELLOW(128, 128, 0), VERY_DARK_YELLOW(64, 64, 0),
GREEN(0, 255, 0), DARK_GREEN(0, 128, 0), VERY_DARK_GREEN(0, 64, 0),
CYAN(0, 255, 255), DARK_CYAN(0, 128, 128), VERY_DARK_CYAN(0, 64, 64),
BLUE(0, 0, 255), DARK_BLUE(0, 0, 128), VERY_DARK_BLUE(0, 0, 64),
MAGENTA(255, 0, 255), DARK_MAGENTA(128, 0, 128), VERY_DARK_MAGENTA(64, 0, 64),
WHITE(255, 255, 255), BLACK(0, 0, 0), BLANK(0, 0, 0, 0);

} // namespace olc
