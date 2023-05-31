#pragma once

#include <olcPGE/olcPGE_export.hpp>

#include <olcPGE/Pixel.hpp>
#include <olcPGE/v2d_generic.hpp>
#include <olcPGE/ImageLoader.hpp>

#include <vector>
#include <memory>


namespace olc
{

// O------------------------------------------------------------------------------O
// | olc::Sprite - An image represented by a 2D array of olc::Pixel               |
// O------------------------------------------------------------------------------O
class OLCPGE_EXPORT Sprite
{
public:
  Sprite();
  Sprite(const std::string& sImageFile);
  Sprite(int32_t w, int32_t h);
  Sprite(olc::Sprite&&);
  Sprite(const olc::Sprite&) = default;
  Sprite& operator=(olc::Sprite&&);
  Sprite& operator=(const olc::Sprite&) = default;
  ~Sprite();

public:
  olc::rcode LoadFromFile(const std::string& sImageFile);

public:
  enum Mode { NORMAL, PERIODIC };
  enum Flip : uint8_t { NONE = 0, HORIZ = 1, VERT = 2 };

  std::vector<olc::Pixel> pColData;

  int32_t width = 0;
  int32_t height = 0;

  Mode modeSample = Mode::NORMAL;

  static Pixel::Mode nPixelMode;

  static std::unique_ptr <olc::ImageLoader> loader;

public:
  void SetSampleMode(olc::Sprite::Mode mode = olc::Sprite::Mode::NORMAL);
  Pixel Sample(float x, float y) const;
  Pixel SampleBL(float u, float v) const;

  void SetPixelMode(Pixel::Mode m);
  Pixel::Mode GetPixelMode();

  Pixel GetPixel(int32_t x, int32_t y) const;
  bool  SetPixel(int32_t x, int32_t y, Pixel p);
  Pixel GetPixel(const olc::vi2d& a) const;
  bool  SetPixel(const olc::vi2d& a, Pixel p);
  void  SetFlipped(const Flip flip);

  Pixel* GetData();

  olc::Sprite* Duplicate();
  olc::Sprite* Duplicate(const olc::vi2d& vPos, const olc::vi2d& vSize);


  // Clears entire draw target to Pixel
  void Clear(Pixel p);

  // Draws a single Pixel
  bool Draw(int32_t x, int32_t y, Pixel p = olc::WHITE, float fBlendFactor = 1.0f);
  bool Draw(const olc::vi2d& pos, Pixel p = olc::WHITE);

  // Draws a line from (x1,y1) to (x2,y2)
  void DrawLine(int32_t x1, int32_t y1, int32_t x2, int32_t y2, Pixel p = olc::WHITE, uint32_t pattern = 0xFFFFFFFF);
  void DrawLine(const olc::vi2d& pos1, const olc::vi2d& pos2, Pixel p = olc::WHITE, uint32_t pattern = 0xFFFFFFFF);

  // Draws a circle located at (x,y) with radius
  void DrawCircle(int32_t x, int32_t y, int32_t radius, Pixel p = olc::WHITE, uint8_t mask = 0xFF);
  void DrawCircle(const olc::vi2d& pos, int32_t radius, Pixel p = olc::WHITE, uint8_t mask = 0xFF);

  // Fills a circle located at (x,y) with radius
  void FillCircle(int32_t x, int32_t y, int32_t radius, Pixel p = olc::WHITE);
  void FillCircle(const olc::vi2d& pos, int32_t radius, Pixel p = olc::WHITE);

  // Draws a rectangle at (x,y) to (x+w,y+h)
  void DrawRect(int32_t x, int32_t y, int32_t w, int32_t h, Pixel p = olc::WHITE);
  void DrawRect(const olc::vi2d& pos, const olc::vi2d& size, Pixel p = olc::WHITE);

  // Fills a rectangle at (x,y) to (x+w,y+h)
  void FillRect(int32_t x, int32_t y, int32_t w, int32_t h, Pixel p = olc::WHITE);
  void FillRect(const olc::vi2d& pos, const olc::vi2d& size, Pixel p = olc::WHITE);

  // Draws a triangle between points (x1,y1), (x2,y2) and (x3,y3)
  void DrawTriangle(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t x3, int32_t y3, Pixel p = olc::WHITE);
  void DrawTriangle(const olc::vi2d& pos1, const olc::vi2d& pos2, const olc::vi2d& pos3, Pixel p = olc::WHITE);

  // Flat fills a triangle between points (x1,y1), (x2,y2) and (x3,y3)
  void FillTriangle(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t x3, int32_t y3, Pixel p = olc::WHITE);
  void FillTriangle(const olc::vi2d& pos1, const olc::vi2d& pos2, const olc::vi2d& pos3, Pixel p = olc::WHITE);

  // Draws an entire sprite at location (x,y)
  void DrawSprite(int32_t x, int32_t y, Sprite* sprite, uint32_t scale = 1, uint8_t flip = olc::Sprite::NONE);
  void DrawSprite(const olc::vi2d& pos, Sprite* sprite, uint32_t scale = 1, uint8_t flip = olc::Sprite::NONE);

  // Draws an area of a sprite at location (x,y), where the
  // selected area is (ox,oy) to (ox+w,oy+h)
  void DrawPartialSprite(int32_t x, int32_t y, Sprite* sprite, int32_t ox, int32_t oy, int32_t w, int32_t h, uint32_t scale = 1, uint8_t flip = olc::Sprite::NONE);
  void DrawPartialSprite(const olc::vi2d& pos, Sprite* sprite, const olc::vi2d& sourcepos, const olc::vi2d& size, uint32_t scale = 1, uint8_t flip = olc::Sprite::NONE);

  // Draws a single line of text - traditional monospaced
  void DrawString(int32_t x, int32_t y, const std::string& sText, Pixel col = olc::WHITE, uint32_t scale = 1);
  void DrawString(const olc::vi2d& pos, const std::string& sText, Pixel col = olc::WHITE, uint32_t scale = 1);
  static olc::vi2d GetTextSize(const std::string& s);

  // Draws a single line of text - non-monospaced
  void DrawStringProp(int32_t x, int32_t y, const std::string& sText, Pixel col = olc::WHITE, uint32_t scale = 1);
  void DrawStringProp(const olc::vi2d& pos, const std::string& sText, Pixel col = olc::WHITE, uint32_t scale = 1);
  static olc::vi2d GetTextSizeProp(const std::string& s);
};

} // namespace olc
