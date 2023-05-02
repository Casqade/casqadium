#pragma once

#include <olcPGE/Sprite.hpp>


struct GLFWcursor;

namespace cqde::types
{

class MouseCursor
{
  GLFWcursor* mCursor {};
  olc::Sprite mSprite {};

public:
  MouseCursor() = default;
  ~MouseCursor();

  void create( const int xhot, const int yhot );
  void destroy();

  void setSprite( olc::Sprite&& );
  void setSprite( const olc::Sprite& );

  olc::Sprite& sprite();
  const olc::Sprite& sprite() const;

  operator GLFWcursor* () const;
};

} // namespace cqde::types
