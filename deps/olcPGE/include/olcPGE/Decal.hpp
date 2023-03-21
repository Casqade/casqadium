#pragma once

#include <olcPGE/olcPGE_export.hpp>

#include <olcPGE/Sprite.hpp>


namespace olc
{
// O------------------------------------------------------------------------------O
// | olc::Decal - A GPU resident storage of an olc::Sprite                        |
// O------------------------------------------------------------------------------O
class OLCPGE_EXPORT Decal
{
public:
  Decal(olc::Sprite* spr, bool filter = false, bool clamp = true);
  Decal(const uint32_t nExistingTextureResource, olc::Sprite* spr);
  virtual ~Decal();
  void Update();

public: // But dont touch
  uint32_t id = 0;
  olc::Sprite* sprite = nullptr;
};

enum class DecalMode
{
  NORMAL,
  ADDITIVE,
  MULTIPLICATIVE,
  STENCIL,
  ILLUMINATE,
  WIREFRAME,
};

} // namespace olc
