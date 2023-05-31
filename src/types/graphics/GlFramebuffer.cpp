#include <cqde/types/graphics/GlFramebuffer.hpp>

#include <cqde/assert.hpp>

#include <glad/gl.h>


namespace cqde::types
{

GlFramebuffer::GlFramebuffer(
  GlFramebuffer&& other )
{
  *this = std::move(other);
}

GlFramebuffer&
GlFramebuffer::operator = (
  GlFramebuffer&& other )
{
  CQDE_ASSERT_DEBUG(isValid() == false, destroy());

  std::swap(mId, other.mId);
  std::swap(mAttachments, other.mAttachments);

  return *this;
}

void
GlFramebuffer::create()
{
  CQDE_ASSERT_DEBUG(isValid() == false, destroy());

  glCreateFramebuffers(1, &mId);
}

void
GlFramebuffer::destroy()
{
  CQDE_ASSERT_DEBUG(isValid() == true, return);

  glDeleteFramebuffers(1, &mId);
  mId = 0;
}

bool
GlFramebuffer::isValid() const
{
  return mId != 0;
}

GLuint
GlFramebuffer::id() const
{
  return mId;
}

} // namespace cqde::types
