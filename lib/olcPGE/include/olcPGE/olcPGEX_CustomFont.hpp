#ifndef __OLC_PGEX_CUSTOMFONT__
#define __OLC_PGEX_CUSTOMFONT__

#include <olcPGE/olcPixelGameEngine.hpp>


namespace olc
{
class CustomFont : public olc::PGEX
{
public:
  CustomFont() = delete;
  CustomFont(const std::string& sFontFile, olc::ResourcePack* pack = nullptr);

  olc::vi2d GetTextSize(const std::string& s);
  olc::vi2d GetTextSizeProp(const std::string& s);

  void DrawStringDecal(const olc::vf2d& pos, const std::string& sText, const Pixel col = olc::WHITE, const olc::vf2d& scale = { 1.0f, 1.0f });
  void DrawRotatedStringDecal(const olc::vf2d& pos, const std::string& sText, const float fAngle, const olc::vf2d& center = {0.0f, 0.0f},
                              const Pixel col = olc::WHITE, const olc::vf2d& scale = { 1.0f, 1.0f });

  void DrawStringPropDecal(const olc::vf2d& pos, const std::string& sText, const Pixel col = olc::WHITE, const olc::vf2d& scale = { 1.0f, 1.0f });
  void DrawRotatedStringPropDecal(const olc::vf2d& pos, const std::string& sText, const float fAngle, const olc::vf2d& center = {0.0f, 0.0f},
                                  const Pixel col = olc::WHITE, const olc::vf2d& scale = { 1.0f, 1.0f });

private:
  std::unique_ptr<olc::Sprite>    fontSprite;
  std::unique_ptr<olc::Decal>     fontDecal;

  float                           fCharWidth;
  float                           fCharHeight;
  std::unordered_map<uint32_t, std::pair<olc::vi2d, olc::vi2d> > mGlyphPositionsMono;
  std::unordered_map<uint32_t, std::pair<olc::vi2d, olc::vi2d> > mGlyphPositionsProp;
};
}

#endif
