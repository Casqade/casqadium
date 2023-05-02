#pragma once

#include <olcPGE/olcPGE_export.hpp>

#include <olcPGE/Decal.hpp>
#include <olcPGE/Sprite.hpp>


namespace olc
{

// O------------------------------------------------------------------------------O
// | olc::Renderable - Convenience class to keep a sprite and decal together      |
// O------------------------------------------------------------------------------O
class OLCPGE_EXPORT Renderable
{
public:
  Renderable() = default;
  Renderable(Renderable&& r) : pSprite(std::move(r.pSprite)), pDecal(std::move(r.pDecal)) {}
  Renderable(const Renderable&) = delete;

  void Create(uint32_t width, uint32_t height, bool filter = false, bool clamp = true);
  void SetDecal(olc::Decal* decal);
  void SetSprite(olc::Sprite* spr);

  olc::Decal* Decal() const;
  olc::Sprite* Sprite() const;

private:
  std::unique_ptr<olc::Sprite> pSprite = nullptr;
  std::unique_ptr<olc::Decal> pDecal = nullptr;
};

} // namespace olc

#pragma endregion
