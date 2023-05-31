#include <olcPGE/Sprite.hpp>

#include <array>
#include <cmath>
#include <cstring>
#include <algorithm>


namespace olc
{

static std::unique_ptr <Sprite> fontSprite {};
static std::unique_ptr <std::vector<olc::vi2d>> vFontSpacing {};

static void InitFontData()
{
  if (  fontSprite != nullptr ||
        vFontSpacing != nullptr )
    return;

  const std::string data
  {
    "?Q`0001oOch0o01o@F40o0<AGD4090LAGD<090@A7ch0?00O7Q`0600>00000000"
    "O000000nOT0063Qo4d8>?7a14Gno94AA4gno94AaOT0>o3`oO400o7QN00000400"
    "Of80001oOg<7O7moBGT7O7lABET024@aBEd714AiOdl717a_=TH013Q>00000000"
    "720D000V?V5oB3Q_HdUoE7a9@DdDE4A9@DmoE4A;Hg]oM4Aj8S4D84@`00000000"
    "OaPT1000Oa`^13P1@AI[?g`1@A=[OdAoHgljA4Ao?WlBA7l1710007l100000000"
    "ObM6000oOfMV?3QoBDD`O7a0BDDH@5A0BDD<@5A0BGeVO5ao@CQR?5Po00000000"
    "Oc``000?Ogij70PO2D]??0Ph2DUM@7i`2DTg@7lh2GUj?0TO0C1870T?00000000"
    "70<4001o?P<7?1QoHg43O;`h@GT0@:@LB@d0>:@hN@L0@?aoN@<0O7ao0000?000"
    "OcH0001SOglLA7mg24TnK7ln24US>0PL24U140PnOgl0>7QgOcH0K71S0000A000"
    "00H00000@Dm1S007@DUSg00?OdTnH7YhOfTL<7Yh@Cl0700?@Ah0300700000000"
    "<008001QL00ZA41a@6HnI<1i@FHLM81M@@0LG81?O`0nC?Y7?`0ZA7Y300080000"
    "O`082000Oh0827mo6>Hn?Wmo?6HnMb11MP08@C11H`08@FP0@@0004@000000000"
    "00P00001Oab00003OcKP0006@6=PMgl<@440MglH@000000`@000001P00000000"
    "Ob@8@@00Ob@8@Ga13R@8Mga172@8?PAo3R@827QoOb@820@0O`0007`0000007P0"
    "O`000P08Od400g`<3V=P0G`673IP0`@3>1`00P@6O`P00g`<O`000GP800000000"
    "?P9PL020O`<`N3R0@E4HC7b0@ET<ATB0@@l6C4B0O`H3N7b0?P01L3R000000020"
  };

  fontSprite = std::make_unique <Sprite> (128, 48);
  int px = 0, py = 0;
  for (size_t b = 0; b < 1024; b += 4)
  {
    uint32_t sym1 = (uint32_t)data[b + 0] - 48;
    uint32_t sym2 = (uint32_t)data[b + 1] - 48;
    uint32_t sym3 = (uint32_t)data[b + 2] - 48;
    uint32_t sym4 = (uint32_t)data[b + 3] - 48;
    uint32_t r = sym1 << 18 | sym2 << 12 | sym3 << 6 | sym4;

    for (int i = 0; i < 24; i++)
    {
      int k = r & (1 << i) ? 255 : 0;
      fontSprite->SetPixel(px, py, olc::Pixel(k, k, k, k));
      if (++py == 48) { px++; py = 0; }
    }
  }

  constexpr std::array<uint8_t, 96> vSpacing =
  {
    {
       0x03,0x25,0x16,0x08,0x07,0x08,0x08,0x04,0x15,0x15,0x08,0x07,0x15,0x07,0x24,0x08,
       0x08,0x17,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x24,0x15,0x06,0x07,0x16,0x17,
       0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x17,0x08,0x08,0x17,0x08,0x08,0x08,
       0x08,0x08,0x08,0x08,0x17,0x08,0x08,0x08,0x08,0x17,0x08,0x15,0x08,0x15,0x08,0x08,
       0x24,0x18,0x17,0x17,0x17,0x17,0x17,0x17,0x17,0x33,0x17,0x17,0x33,0x18,0x17,0x17,
       0x17,0x17,0x17,0x17,0x07,0x17,0x17,0x18,0x18,0x17,0x17,0x07,0x33,0x07,0x08,0x00,
    }
  };

  vFontSpacing = std::make_unique <std::vector<olc::vi2d>> ();

  for (auto c : vSpacing)
    vFontSpacing->push_back({ c >> 4, c & 15 });
}


Pixel::Mode Sprite::nPixelMode = Pixel::NORMAL;

Sprite::Sprite()
{ width = 0; height = 0; }

Sprite::Sprite(const std::string& sImageFile)
{ LoadFromFile(sImageFile); }

Sprite::Sprite(int32_t w, int32_t h)
{
  width = w;		height = h;
  pColData.resize(width * height);
  pColData.resize(width * height, nDefaultPixel);
}

Sprite::Sprite(Sprite&& other)
{
  *this = std::move(other);
}

Sprite& Sprite::operator=(Sprite&& other)
{
  pColData = std::move(other.pColData);
  width = other.width;
  height = other.height;
  modeSample = other.modeSample;

  return *this;
}

Sprite::~Sprite()
{ pColData.clear();	}

void Sprite::SetSampleMode(olc::Sprite::Mode mode)
{ modeSample = mode; }

Pixel Sprite::GetPixel(const olc::vi2d& a) const
{ return GetPixel(a.x, a.y); }

bool Sprite::SetPixel(const olc::vi2d& a, Pixel p)
{ return SetPixel(a.x, a.y, p); }

Pixel Sprite::GetPixel(int32_t x, int32_t y) const
{
  if (modeSample == olc::Sprite::Mode::NORMAL)
    {
      if (x >= 0 && x < width && y >= 0 && y < height)
        return pColData[y * width + x];
      else
        return Pixel(0, 0, 0, 0);
    }
  else
    {
      return pColData[abs(y % height) * width + abs(x % width)];
    }
}

bool Sprite::SetPixel(int32_t x, int32_t y, Pixel p)
{
  if (x >= 0 && x < width && y >= 0 && y < height)
    {
      pColData[y * width + x] = p;
      return true;
    }
  else
    return false;
}

void Sprite::SetFlipped(const Flip flip)
{
  const bool flipH = flip & olc::Sprite::Flip::HORIZ;
  const bool flipV = flip & olc::Sprite::Flip::VERT;

  if ( flipH == false && flipV == false )
    return;

  if ( flipH == true && flipV == true ||
       flipH == true && height < 2 ||
       flipV == true && width < 2 )
  {
    std::reverse(pColData.begin(), pColData.end());
    return;
  }

  const auto dataTmp {std::move(pColData)};
  pColData.clear();

  int32_t xDir = 1;
  int32_t yDir = 1;
  int32_t xInitial = 0;
  int32_t yInitial = 0;

  if ( flipH == true )
  {
    xDir = -1;
    xInitial = width - 1;
  }

  if ( flipV == true )
  {
    yDir = -1;
    yInitial = height - 1;
  }

  for ( int32_t yi = 0, y = yInitial;
        yi < height; ++yi, y += yDir )
  {
    for ( int32_t xi = 0, x = xInitial;
          xi < width; ++xi, x += xDir )
      pColData.push_back(dataTmp[y * width + x]);
  }
}

Pixel Sprite::Sample(float x, float y) const
{
  int32_t sx = std::min((int32_t)((x * (float)width)), width - 1);
  int32_t sy = std::min((int32_t)((y * (float)height)), height - 1);
  return GetPixel(sx, sy);
}

Pixel Sprite::SampleBL(float u, float v) const
{
  u = u * width - 0.5f;
  v = v * height - 0.5f;
  int x = (int)std::floor(u); // cast to int rounds toward zero, not downward
  int y = (int)std::floor(v); // Thanks @joshinils
  float u_ratio = u - x;
  float v_ratio = v - y;
  float u_opposite = 1 - u_ratio;
  float v_opposite = 1 - v_ratio;

  olc::Pixel p1 = GetPixel(std::max(x, 0), std::max(y, 0));
  olc::Pixel p2 = GetPixel(std::min(x + 1, (int)width - 1), std::max(y, 0));
  olc::Pixel p3 = GetPixel(std::max(x, 0), std::min(y + 1, (int)height - 1));
  olc::Pixel p4 = GetPixel(std::min(x + 1, (int)width - 1), std::min(y + 1, (int)height - 1));

  return olc::Pixel(
        (uint8_t)((p1.r * u_opposite + p2.r * u_ratio) * v_opposite + (p3.r * u_opposite + p4.r * u_ratio) * v_ratio),
        (uint8_t)((p1.g * u_opposite + p2.g * u_ratio) * v_opposite + (p3.g * u_opposite + p4.g * u_ratio) * v_ratio),
        (uint8_t)((p1.b * u_opposite + p2.b * u_ratio) * v_opposite + (p3.b * u_opposite + p4.b * u_ratio) * v_ratio));
}

Pixel* Sprite::GetData()
{ return pColData.data(); }


olc::rcode Sprite::LoadFromFile(const std::string& sImageFile)
{
  return loader->LoadImageResource(this, sImageFile);
}

olc::Sprite* Sprite::Duplicate()
{
  olc::Sprite* spr = new olc::Sprite(width, height);
  std::memcpy(spr->GetData(), GetData(), width * height * sizeof(olc::Pixel));
  spr->modeSample = modeSample;
  return spr;
}

olc::Sprite* Sprite::Duplicate(const olc::vi2d& vPos, const olc::vi2d& vSize)
{
  olc::Sprite* spr = new olc::Sprite(vSize.x, vSize.y);
  for (int y = 0; y < vSize.y; y++)
    for (int x = 0; x < vSize.x; x++)
      spr->SetPixel(x, y, GetPixel(vPos.x + x, vPos.y + y));
  return spr;
}

void Sprite::SetPixelMode(Pixel::Mode m)
{ nPixelMode = m; }

Pixel::Mode Sprite::GetPixelMode()
{ return nPixelMode; }


void Sprite::Clear(Pixel p)
{
  int pixels = this->width * this->height;
  for (int i = 0; i < pixels; i++)
    pColData[i] = p;
}

bool Sprite::Draw(const olc::vi2d& pos, Pixel p)
{ return Draw(pos.x, pos.y, p); }

// This is it, the critical function that plots a pixel
bool Sprite::Draw(int32_t x, int32_t y, Pixel p, float fBlendFactor)
{
  if (nPixelMode == Pixel::NORMAL)
    {
      return SetPixel(x, y, p);
    }

  if (nPixelMode == Pixel::MASK)
    {
      if (p.a == 255)
        return SetPixel(x, y, p);
    }

  if (nPixelMode == Pixel::ALPHA)
    {
      Pixel d = GetPixel(x, y);
      float a = (float)(p.a / 255.0f) * fBlendFactor;
      float c = 1.0f - a;
      float r = a * (float)p.r + c * (float)d.r;
      float g = a * (float)p.g + c * (float)d.g;
      float b = a * (float)p.b + c * (float)d.b;
      return SetPixel(x, y, Pixel((uint8_t)r, (uint8_t)g, (uint8_t)b/*, (uint8_t)(p.a * fBlendFactor)*/));
    }

  return false;
}


void Sprite::DrawLine(const olc::vi2d& pos1, const olc::vi2d& pos2, Pixel p, uint32_t pattern)
{ DrawLine(pos1.x, pos1.y, pos2.x, pos2.y, p, pattern); }

void Sprite::DrawLine(int32_t x1, int32_t y1, int32_t x2, int32_t y2, Pixel p, uint32_t pattern)
{
  int x, y, dx, dy, dx1, dy1, px, py, xe, ye, i;
  dx = x2 - x1; dy = y2 - y1;

  auto rol = [&](void) { pattern = (pattern << 1) | (pattern >> 31); return pattern & 1; };

  // straight lines idea by gurkanctn
  if (dx == 0) // Line is vertical
    {
      if (y2 < y1) std::swap(y1, y2);
      for (y = y1; y <= y2; y++) if (rol()) Draw(x1, y, p);
      return;
    }

  if (dy == 0) // Line is horizontal
    {
      if (x2 < x1) std::swap(x1, x2);
      for (x = x1; x <= x2; x++) if (rol()) Draw(x, y1, p);
      return;
    }

  // Line is Funk-aye
  dx1 = abs(dx); dy1 = abs(dy);
  px = 2 * dy1 - dx1;	py = 2 * dx1 - dy1;
  if (dy1 <= dx1)
    {
      if (dx >= 0)
        {
          x = x1; y = y1; xe = x2;
        }
      else
        {
          x = x2; y = y2; xe = x1;
        }

      if (rol()) Draw(x, y, p);

      for (i = 0; x < xe; i++)
        {
          x = x + 1;
          if (px < 0)
            px = px + 2 * dy1;
          else
            {
              if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0)) y = y + 1; else y = y - 1;
              px = px + 2 * (dy1 - dx1);
            }
          if (rol()) Draw(x, y, p);
        }
    }
  else
    {
      if (dy >= 0)
        {
          x = x1; y = y1; ye = y2;
        }
      else
        {
          x = x2; y = y2; ye = y1;
        }

      if (rol()) Draw(x, y, p);

      for (i = 0; y < ye; i++)
        {
          y = y + 1;
          if (py <= 0)
            py = py + 2 * dx1;
          else
            {
              if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0)) x = x + 1; else x = x - 1;
              py = py + 2 * (dx1 - dy1);
            }
          if (rol()) Draw(x, y, p);
        }
    }
}

void Sprite::DrawCircle(const olc::vi2d& pos, int32_t radius, Pixel p, uint8_t mask)
{ DrawCircle(pos.x, pos.y, radius, p, mask); }

void Sprite::DrawCircle(int32_t x, int32_t y, int32_t radius, Pixel p, uint8_t mask)
{ // Thanks to IanM-Matrix1 #PR121
  if (radius < 0 || x < -radius || y < -radius || x - this->width > radius || y - this->height > radius)
    return;

  if (radius > 0)
    {
      int x0 = 0;
      int y0 = radius;
      int d = 3 - 2 * radius;

      while (y0 >= x0) // only formulate 1/8 of circle
        {
          // Draw even octants
          if (mask & 0x01) Draw(x + x0, y - y0, p);// Q6 - upper right right
          if (mask & 0x04) Draw(x + y0, y + x0, p);// Q4 - lower lower right
          if (mask & 0x10) Draw(x - x0, y + y0, p);// Q2 - lower left left
          if (mask & 0x40) Draw(x - y0, y - x0, p);// Q0 - upper upper left
          if (x0 != 0 && x0 != y0)
            {
              if (mask & 0x02) Draw(x + y0, y - x0, p);// Q7 - upper upper right
              if (mask & 0x08) Draw(x + x0, y + y0, p);// Q5 - lower right right
              if (mask & 0x20) Draw(x - y0, y + x0, p);// Q3 - lower lower left
              if (mask & 0x80) Draw(x - x0, y - y0, p);// Q1 - upper left left
            }

          if (d < 0)
            d += 4 * x0++ + 6;
          else
            d += 4 * (x0++ - y0--) + 10;
        }
    }
  else
    Draw(x, y, p);
}

void Sprite::FillCircle(const olc::vi2d& pos, int32_t radius, Pixel p)
{ FillCircle(pos.x, pos.y, radius, p); }

void Sprite::FillCircle(int32_t x, int32_t y, int32_t radius, Pixel p)
{ // Thanks to IanM-Matrix1 #PR121
  if (radius < 0 || x < -radius || y < -radius || x - this->width > radius || y - this->height > radius)
    return;

  if (radius > 0)
    {
      int x0 = 0;
      int y0 = radius;
      int d = 3 - 2 * radius;

      auto drawline = [&](int sx, int ex, int y)
      {
          for (int x = sx; x <= ex; x++)
            Draw(x, y, p);
        };

      while (y0 >= x0)
        {
          drawline(x - y0, x + y0, y - x0);
          if (x0 > 0)	drawline(x - y0, x + y0, y + x0);

          if (d < 0)
            d += 4 * x0++ + 6;
          else
            {
              if (x0 != y0)
                {
                  drawline(x - x0, x + x0, y - y0);
                  drawline(x - x0, x + x0, y + y0);
                }
              d += 4 * (x0++ - y0--) + 10;
            }
        }
    }
  else
    Draw(x, y, p);
}

void Sprite::DrawRect(const olc::vi2d& pos, const olc::vi2d& size, Pixel p)
{ DrawRect(pos.x, pos.y, size.x, size.y, p); }

void Sprite::DrawRect(int32_t x, int32_t y, int32_t w, int32_t h, Pixel p)
{
  DrawLine(x, y, x + w, y, p);
  DrawLine(x + w, y, x + w, y + h, p);
  DrawLine(x + w, y + h, x, y + h, p);
  DrawLine(x, y + h, x, y, p);
}

void Sprite::FillRect(const olc::vi2d& pos, const olc::vi2d& size, Pixel p)
{ FillRect(pos.x, pos.y, size.x, size.y, p); }

void Sprite::FillRect(int32_t x, int32_t y, int32_t w, int32_t h, Pixel p)
{
  int32_t x2 = x + w;
  int32_t y2 = y + h;

  if (x < 0) x = 0;
  if (x >= this->width) x = this->width;
  if (y < 0) y = 0;
  if (y >= this->height) y = this->height;

  if (x2 < 0) x2 = 0;
  if (x2 >= this->width) x2 = this->width;
  if (y2 < 0) y2 = 0;
  if (y2 >= this->height) y2 = this->height;

  for (int i = x; i < x2; i++)
    for (int j = y; j < y2; j++)
      Draw(i, j, p);
}

void Sprite::DrawTriangle(const olc::vi2d& pos1, const olc::vi2d& pos2, const olc::vi2d& pos3, Pixel p)
{ DrawTriangle(pos1.x, pos1.y, pos2.x, pos2.y, pos3.x, pos3.y, p); }

void Sprite::DrawTriangle(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t x3, int32_t y3, Pixel p)
{
  DrawLine(x1, y1, x2, y2, p);
  DrawLine(x2, y2, x3, y3, p);
  DrawLine(x3, y3, x1, y1, p);
}

void Sprite::FillTriangle(const olc::vi2d& pos1, const olc::vi2d& pos2, const olc::vi2d& pos3, Pixel p)
{ FillTriangle(pos1.x, pos1.y, pos2.x, pos2.y, pos3.x, pos3.y, p); }

// https://www.avrfreaks.net/sites/default/files/triangles.c
void Sprite::FillTriangle(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t x3, int32_t y3, Pixel p)
{
  auto drawline = [&](int sx, int ex, int ny) { for (int i = sx; i <= ex; i++) Draw(i, ny, p); };

  int t1x, t2x, y, minx, maxx, t1xp, t2xp;
  bool changed1 = false;
  bool changed2 = false;
  int signx1, signx2, dx1, dy1, dx2, dy2;
  int e1, e2;
  // Sort vertices
  if (y1 > y2) { std::swap(y1, y2); std::swap(x1, x2); }
  if (y1 > y3) { std::swap(y1, y3); std::swap(x1, x3); }
  if (y2 > y3) { std::swap(y2, y3); std::swap(x2, x3); }

  t1x = t2x = x1; y = y1;   // Starting points
  dx1 = (int)(x2 - x1);
  if (dx1 < 0) { dx1 = -dx1; signx1 = -1; }
  else signx1 = 1;
  dy1 = (int)(y2 - y1);

  dx2 = (int)(x3 - x1);
  if (dx2 < 0) { dx2 = -dx2; signx2 = -1; }
  else signx2 = 1;
  dy2 = (int)(y3 - y1);

  if (dy1 > dx1) { std::swap(dx1, dy1); changed1 = true; }
  if (dy2 > dx2) { std::swap(dy2, dx2); changed2 = true; }

  e2 = (int)(dx2 >> 1);
  // Flat top, just process the second half
  if (y1 == y2) goto next;
  e1 = (int)(dx1 >> 1);

  for (int i = 0; i < dx1;) {
      t1xp = 0; t2xp = 0;
      if (t1x < t2x) { minx = t1x; maxx = t2x; }
      else { minx = t2x; maxx = t1x; }
      // process first line until y value is about to change
      while (i < dx1) {
          i++;
          e1 += dy1;
          while (e1 >= dx1) {
              e1 -= dx1;
              if (changed1) t1xp = signx1;//t1x += signx1;
              else          goto next1;
            }
          if (changed1) break;
          else t1x += signx1;
        }
      // Move line
next1:
      // process second line until y value is about to change
      while (1) {
          e2 += dy2;
          while (e2 >= dx2) {
              e2 -= dx2;
              if (changed2) t2xp = signx2;//t2x += signx2;
              else          goto next2;
            }
          if (changed2)     break;
          else              t2x += signx2;
        }
next2:
      if (minx > t1x) minx = t1x;
      if (minx > t2x) minx = t2x;
      if (maxx < t1x) maxx = t1x;
      if (maxx < t2x) maxx = t2x;
      drawline(minx, maxx, y);    // Draw line from min to max points found on the y
      // Now increase y
      if (!changed1) t1x += signx1;
      t1x += t1xp;
      if (!changed2) t2x += signx2;
      t2x += t2xp;
      y += 1;
      if (y == y2) break;
    }
next:
  // Second half
  dx1 = (int)(x3 - x2); if (dx1 < 0) { dx1 = -dx1; signx1 = -1; }
  else signx1 = 1;
  dy1 = (int)(y3 - y2);
  t1x = x2;

  if (dy1 > dx1) {   // swap values
      std::swap(dy1, dx1);
      changed1 = true;
    }
  else changed1 = false;

  e1 = (int)(dx1 >> 1);

  for (int i = 0; i <= dx1; i++) {
      t1xp = 0; t2xp = 0;
      if (t1x < t2x) { minx = t1x; maxx = t2x; }
      else { minx = t2x; maxx = t1x; }
      // process first line until y value is about to change
      while (i < dx1) {
          e1 += dy1;
          while (e1 >= dx1) {
              e1 -= dx1;
              if (changed1) { t1xp = signx1; break; }//t1x += signx1;
              else          goto next3;
            }
          if (changed1) break;
          else   	   	  t1x += signx1;
          if (i < dx1) i++;
        }
next3:
      // process second line until y value is about to change
      while (t2x != x3) {
          e2 += dy2;
          while (e2 >= dx2) {
              e2 -= dx2;
              if (changed2) t2xp = signx2;
              else          goto next4;
            }
          if (changed2)     break;
          else              t2x += signx2;
        }
next4:

      if (minx > t1x) minx = t1x;
      if (minx > t2x) minx = t2x;
      if (maxx < t1x) maxx = t1x;
      if (maxx < t2x) maxx = t2x;
      drawline(minx, maxx, y);
      if (!changed1) t1x += signx1;
      t1x += t1xp;
      if (!changed2) t2x += signx2;
      t2x += t2xp;
      y += 1;
      if (y > y3) return;
    }
}

void Sprite::DrawSprite(const olc::vi2d& pos, Sprite* sprite, uint32_t scale, uint8_t flip)
{ DrawSprite(pos.x, pos.y, sprite, scale, flip); }

void Sprite::DrawSprite(int32_t x, int32_t y, Sprite* sprite, uint32_t scale, uint8_t flip)
{
  if (sprite == nullptr)
    return;

  int32_t fxs = 0, fxm = 1, fx = 0;
  int32_t fys = 0, fym = 1, fy = 0;
  if (flip & olc::Sprite::Flip::HORIZ) { fxs = sprite->width - 1; fxm = -1; }
  if (flip & olc::Sprite::Flip::VERT) { fys = sprite->height - 1; fym = -1; }

  if (scale > 1)
    {
      fx = fxs;
      for (int32_t i = 0; i < sprite->width; i++, fx += fxm)
        {
          fy = fys;
          for (int32_t j = 0; j < sprite->height; j++, fy += fym)
            for (uint32_t is = 0; is < scale; is++)
              for (uint32_t js = 0; js < scale; js++)
                Draw(x + (i * scale) + is, y + (j * scale) + js, sprite->GetPixel(fx, fy));
        }
    }
  else
    {
      fx = fxs;
      for (int32_t i = 0; i < sprite->width; i++, fx += fxm)
        {
          fy = fys;
          for (int32_t j = 0; j < sprite->height; j++, fy += fym)
            Draw(x + i, y + j, sprite->GetPixel(fx, fy));
        }
    }
}

void Sprite::DrawPartialSprite(const olc::vi2d& pos, Sprite* sprite, const olc::vi2d& sourcepos, const olc::vi2d& size, uint32_t scale, uint8_t flip)
{ DrawPartialSprite(pos.x, pos.y, sprite, sourcepos.x, sourcepos.y, size.x, size.y, scale, flip); }

void Sprite::DrawPartialSprite(int32_t x, int32_t y, Sprite* sprite, int32_t ox, int32_t oy, int32_t w, int32_t h, uint32_t scale, uint8_t flip)
{
  if (sprite == nullptr)
    return;

  int32_t fxs = 0, fxm = 1, fx = 0;
  int32_t fys = 0, fym = 1, fy = 0;
  if (flip & olc::Sprite::Flip::HORIZ) { fxs = w - 1; fxm = -1; }
  if (flip & olc::Sprite::Flip::VERT) { fys = h - 1; fym = -1; }

  if (scale > 1)
    {
      fx = fxs;
      for (int32_t i = 0; i < w; i++, fx += fxm)
        {
          fy = fys;
          for (int32_t j = 0; j < h; j++, fy += fym)
            for (uint32_t is = 0; is < scale; is++)
              for (uint32_t js = 0; js < scale; js++)
                Draw(x + (i * scale) + is, y + (j * scale) + js, sprite->GetPixel(fx + ox, fy + oy));
        }
    }
  else
    {
      fx = fxs;
      for (int32_t i = 0; i < w; i++, fx += fxm)
        {
          fy = fys;
          for (int32_t j = 0; j < h; j++, fy += fym)
            Draw(x + i, y + j, sprite->GetPixel(fx + ox, fy + oy));
        }
    }
}

olc::vi2d Sprite::GetTextSize(const std::string& s)
{
  olc::vi2d size = { 0,1 };
  olc::vi2d pos = { 0,1 };
  for (auto c : s)
    {
      if (c == '\n') { pos.y++;  pos.x = 0; }
      else pos.x++;
      size.x = std::max(size.x, pos.x);
      size.y = std::max(size.y, pos.y);
    }
  return size * 8;
}

olc::vi2d Sprite::GetTextSizeProp(const std::string& s)
{
  InitFontData();

  const auto& fontSpacing = *vFontSpacing.get();

  olc::vi2d size = { 0,1 };
  olc::vi2d pos = { 0,1 };
  for (auto c : s)
    {
      if (c == '\n') { pos.y += 1;  pos.x = 0; }
      else pos.x += fontSpacing[c - 32].y;
      size.x = std::max(size.x, pos.x);
      size.y = std::max(size.y, pos.y);
    }

  size.y *= 8;
  return size;
}

void Sprite::DrawString(const olc::vi2d& pos, const std::string& sText, Pixel col, uint32_t scale)
{ DrawString(pos.x, pos.y, sText, col, scale); }

void Sprite::DrawString(int32_t x, int32_t y, const std::string& sText, Pixel col, uint32_t scale)
{
  InitFontData();

  int32_t sx = 0;
  int32_t sy = 0;
  Pixel::Mode m = nPixelMode;

  // Thanks @tucna, spotted bug with col.ALPHA :P
  if (col.a != 255)
    SetPixelMode(Pixel::ALPHA);
  else
    SetPixelMode(Pixel::MASK);

  for (auto c : sText)
    {
      if (c == '\n')
        {
          sx = 0; sy += 8 * scale;
        }
      else
        {
          int32_t ox = (c - 32) % 16;
          int32_t oy = (c - 32) / 16;

          if (scale > 1)
            {
              for (uint32_t i = 0; i < 8; i++)
                for (uint32_t j = 0; j < 8; j++)
                  if (fontSprite->GetPixel(i + ox * 8, j + oy * 8).r > 0)
                    for (uint32_t is = 0; is < scale; is++)
                      for (uint32_t js = 0; js < scale; js++)
                        Draw(x + sx + (i * scale) + is, y + sy + (j * scale) + js, col);
            }
          else
            {
              for (uint32_t i = 0; i < 8; i++)
                for (uint32_t j = 0; j < 8; j++)
                  if (fontSprite->GetPixel(i + ox * 8, j + oy * 8).r > 0)
                    Draw(x + sx + i, y + sy + j, col);
            }
          sx += 8 * scale;
        }
    }
  SetPixelMode(m);
}

void Sprite::DrawStringProp(const olc::vi2d& pos, const std::string& sText, Pixel col, uint32_t scale)
{ DrawStringProp(pos.x, pos.y, sText, col, scale); }

void Sprite::DrawStringProp(int32_t x, int32_t y, const std::string& sText, Pixel col, uint32_t scale)
{
  InitFontData();

  const auto& fontSpacing = *vFontSpacing.get();

  int32_t sx = 0;
  int32_t sy = 0;
  Pixel::Mode m = nPixelMode;

  if (col.a != 255)
    SetPixelMode(Pixel::ALPHA);
  else
    SetPixelMode(Pixel::MASK);

  for (auto c : sText)
    {
      if (c == '\n')
        {
          sx = 0; sy += 8 * scale;
        }
      else
        {
          int32_t ox = (c - 32) % 16;
          int32_t oy = (c - 32) / 16;

          if (scale > 1)
            {
              for (int32_t i = 0; i < fontSpacing[c - 32].y; i++)
                for (int32_t j = 0; j < 8; j++)
                  if (fontSprite->GetPixel(i + ox * 8 + fontSpacing[c - 32].x, j + oy * 8).r > 0)
                    for (int32_t is = 0; is < int(scale); is++)
                      for (int32_t js = 0; js < int(scale); js++)
                        Draw(x + sx + (i * scale) + is, y + sy + (j * scale) + js, col);
            }
          else
            {
              for (int32_t i = 0; i < fontSpacing[c - 32].y; i++)
                for (int32_t j = 0; j < 8; j++)
                  if (fontSprite->GetPixel(i + ox * 8 + fontSpacing[c - 32].x, j + oy * 8).r > 0)
                    Draw(x + sx + i, y + sy + j, col);
            }
          sx += fontSpacing[c - 32].y * scale;
        }
    }
  SetPixelMode(m);
}

} // namespace olc
