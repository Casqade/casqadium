#include <olcPGE/Renderable.hpp>


namespace olc
{

void Renderable::Create(uint32_t width, uint32_t height, bool filter, bool clamp)
{
  pSprite = std::make_unique <olc::Sprite>(width, height);
  pDecal = std::make_unique <olc::Decal>(pSprite.get(), filter, clamp);
}

void Renderable::SetDecal(olc::Decal* decal)
{
  pDecal.reset(decal);
}

void Renderable::SetSprite(olc::Sprite* spr)
{
  pSprite.reset(spr);

  if (pDecal == nullptr)
    return;

  pDecal->sprite = spr;
  pDecal->Update();
}

olc::Decal* Renderable::Decal() const
{ return pDecal.get(); }

olc::Sprite* Renderable::Sprite() const
{ return pSprite.get(); }

} // namespace olc
