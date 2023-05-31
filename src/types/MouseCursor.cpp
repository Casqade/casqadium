#include <cqde/types/MouseCursor.hpp>

#include <olcPGE/Sprite.hpp>

#include <GLFW/glfw3.h>


namespace cqde::types
{

MouseCursor::~MouseCursor()
{
  destroy();
}

void
MouseCursor::create(
  const int xhot,
  const int yhot )
{
  if ( mSprite.pColData.empty() == true )
    return;

  const GLFWimage cursorImage
  {
    mSprite.width,
    mSprite.height,
    &mSprite.pColData.front().r,
  };

  mCursor = glfwCreateCursor(
    &cursorImage,
    xhot, yhot );
}

void
MouseCursor::destroy()
{
  glfwDestroyCursor(mCursor);
  mCursor = nullptr;
}

MouseCursor::operator
GLFWcursor* () const
{
  return mCursor;
}

void
MouseCursor::setSprite(
  olc::Sprite&& sprite )
{
  mSprite = std::move(sprite);
}

void
MouseCursor::setSprite(
  const olc::Sprite& sprite )
{
  mSprite = sprite;
}

olc::Sprite&
MouseCursor::sprite()
{
  return mSprite;
}

const olc::Sprite&
MouseCursor::sprite() const
{
  return mSprite;
}

} // namespace cqde::types
