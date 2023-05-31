#include <olcPGE/Pixel.hpp>

#include <cmath>
#include <algorithm>


namespace olc
{

Pixel::Pixel()
{ r = 0; g = 0; b = 0; a = nDefaultAlpha; }

Pixel::Pixel(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha)
{ n = red | (green << 8) | (blue << 16) | (alpha << 24); } // Thanks jarekpelczar

Pixel::Pixel(uint32_t p)
{ n = p; }

bool Pixel::operator==(const Pixel& p) const
{ return n == p.n; }

bool Pixel::operator!=(const Pixel& p) const
{ return n != p.n; }

Pixel  Pixel::operator * (const float i) const
{
  float fR = std::min(255.0f, std::max(0.0f, float(r) * i));
  float fG = std::min(255.0f, std::max(0.0f, float(g) * i));
  float fB = std::min(255.0f, std::max(0.0f, float(b) * i));
  return Pixel(uint8_t(fR), uint8_t(fG), uint8_t(fB), a);
}

Pixel  Pixel::operator / (const float i) const
{
  float fR = std::min(255.0f, std::max(0.0f, float(r) / i));
  float fG = std::min(255.0f, std::max(0.0f, float(g) / i));
  float fB = std::min(255.0f, std::max(0.0f, float(b) / i));
  return Pixel(uint8_t(fR), uint8_t(fG), uint8_t(fB), a);
}

Pixel& Pixel::operator *=(const float i)
{
  this->r = uint8_t(std::min(255.0f, std::max(0.0f, float(r) * i)));
  this->g = uint8_t(std::min(255.0f, std::max(0.0f, float(g) * i)));
  this->b = uint8_t(std::min(255.0f, std::max(0.0f, float(b) * i)));
  return *this;
}

Pixel& Pixel::operator /=(const float i)
{
  this->r = uint8_t(std::min(255.0f, std::max(0.0f, float(r) / i)));
  this->g = uint8_t(std::min(255.0f, std::max(0.0f, float(g) / i)));
  this->b = uint8_t(std::min(255.0f, std::max(0.0f, float(b) / i)));
  return *this;
}

Pixel  Pixel::operator + (const Pixel& p) const
{
  uint8_t nR = uint8_t(std::min(255, std::max(0, int(r) + int(p.r))));
  uint8_t nG = uint8_t(std::min(255, std::max(0, int(g) + int(p.g))));
  uint8_t nB = uint8_t(std::min(255, std::max(0, int(b) + int(p.b))));
  return Pixel(nR, nG, nB, a);
}

Pixel  Pixel::operator - (const Pixel& p) const
{
  uint8_t nR = uint8_t(std::min(255, std::max(0, int(r) - int(p.r))));
  uint8_t nG = uint8_t(std::min(255, std::max(0, int(g) - int(p.g))));
  uint8_t nB = uint8_t(std::min(255, std::max(0, int(b) - int(p.b))));
  return Pixel(nR, nG, nB, a);
}

Pixel& Pixel::operator += (const Pixel& p)
{
  this->r = uint8_t(std::min(255, std::max(0, int(r) + int(p.r))));
  this->g = uint8_t(std::min(255, std::max(0, int(g) + int(p.g))));
  this->b = uint8_t(std::min(255, std::max(0, int(b) + int(p.b))));
  return *this;
}

Pixel& Pixel::operator -= (const Pixel& p) // Thanks Au Lit
{
  this->r = uint8_t(std::min(255, std::max(0, int(r) - int(p.r))));
  this->g = uint8_t(std::min(255, std::max(0, int(g) - int(p.g))));
  this->b = uint8_t(std::min(255, std::max(0, int(b) - int(p.b))));
  return *this;
}

Pixel Pixel::inv() const
{
  uint8_t nR = uint8_t(std::min(255, std::max(0, 255 - int(r))));
  uint8_t nG = uint8_t(std::min(255, std::max(0, 255 - int(g))));
  uint8_t nB = uint8_t(std::min(255, std::max(0, 255 - int(b))));
  return Pixel(nR, nG, nB, a);
}

Pixel PixelF(float red, float green, float blue, float alpha)
{ return Pixel(uint8_t(red * 255.0f), uint8_t(green * 255.0f), uint8_t(blue * 255.0f), uint8_t(alpha * 255.0f)); }

Pixel PixelLerp(const olc::Pixel& p1, const olc::Pixel& p2, float t)
{ return (p2 * t) + p1 * (1.0f - t); }

} // namespace olc
