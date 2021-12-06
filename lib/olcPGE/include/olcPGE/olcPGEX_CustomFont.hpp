#ifndef __OLC_PGEX_CUSTOMFONT__
#define __OLC_PGEX_CUSTOMFONT__

#include <olcPGE/olcPixelGameEngine.hpp>

#include <unordered_map>


namespace olc
{
class CustomFont : public olc::PGEX
{
public:
  CustomFont() = delete;
  CustomFont(const std::string& sFontFile, olc::ResourcePack* pack = nullptr);

  olc::vu2d GetTextSize(const std::string& s) const;
  olc::vu2d GetTextSizeProp(const std::string& s) const;

  void DrawStringDecal(const olc::vf2d& pos, const std::string& sText, const Pixel col = olc::WHITE, const olc::vf2d& scale = { 1.0f, 1.0f }) const;
  void DrawRotatedStringDecal(const olc::vf2d& pos, const std::string& sText, const float fAngle, const olc::vf2d& center = {0.0f, 0.0f},
                              const Pixel col = olc::WHITE, const olc::vf2d& scale = { 1.0f, 1.0f }) const;

  void DrawStringPropDecal(const olc::vf2d& pos, const std::string& sText, const Pixel col = olc::WHITE, const olc::vf2d& scale = { 1.0f, 1.0f }) const;
  void DrawRotatedStringPropDecal(const olc::vf2d& pos, const std::string& sText, const float fAngle, const olc::vf2d& center = {0.0f, 0.0f},
                                  const Pixel col = olc::WHITE, const olc::vf2d& scale = { 1.0f, 1.0f }) const;

private:
  std::unique_ptr<olc::Sprite>    fontSprite;
  std::unique_ptr<olc::Decal>     fontDecal;

  olc::vu2d fCharSize;
  std::unordered_map<uint32_t, std::pair<olc::vu2d, olc::vu2d> > mGlyphPositionsMono;
  std::unordered_map<uint32_t, std::pair<olc::vu2d, olc::vu2d> > mGlyphPositionsProp;
};

void DrawFontDecal(const olc::vf2d& pos, const olc::CustomFont*, const std::string& sText, const Pixel col = olc::WHITE, const olc::vf2d& scale = { 1.0f, 1.0f });
void DrawRotatedFontDecal(const olc::vf2d& pos, const olc::CustomFont*, const std::string& sText, const float fAngle, const olc::vf2d& center = {0.0f, 0.0f},
                            const Pixel col = olc::WHITE, const olc::vf2d& scale = { 1.0f, 1.0f });

void DrawFontPropDecal(const olc::vf2d& pos, const olc::CustomFont*, const std::string& sText, const Pixel col = olc::WHITE, const olc::vf2d& scale = { 1.0f, 1.0f });
void DrawRotatedFontPropDecal(const olc::vf2d& pos, const olc::CustomFont*, const std::string& sText, const float fAngle, const olc::vf2d& center = {0.0f, 0.0f},
                                const Pixel col = olc::WHITE, const olc::vf2d& scale = { 1.0f, 1.0f });

}

#endif
