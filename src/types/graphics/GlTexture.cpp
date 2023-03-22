#include <cqde/types/graphics/GlTexture.hpp>

#include <cqde/common.hpp>

#include <glad/gl.h>

#include <glm/vec2.hpp>


namespace cqde::types
{

void
GlTexture::create(
  const glm::u16vec2& size,
  const GLenum type,
  const GLenum internalFormat,
  const size_t levels,
  const SamplerParams& params )
{
  CQDE_ASSERT_DEBUG(isValid() == false, return);

  glCreateTextures(type, 1, &mId);

  for ( const auto& [name, param] : params )
    glTextureParameteri(mId, name, param);

  glTextureStorage2D( mId,
    levels, internalFormat,
    size.x, size.y );

  mSize =
  {
    size.x,
    size.y,
  };
}

void
GlTexture::destroy()
{
  CQDE_ASSERT_DEBUG(isValid() == true, return);

  glDeleteTextures(1, &mId);
  mId = 0;
}

void
GlTexture::store(
  const size_t level,
  const glm::u16vec2& offset,
  const glm::u16vec2& size,
  const GLenum format,
  const GLenum type,
  const void* data )
{
  CQDE_ASSERT_DEBUG(isValid() == true, return);
  CQDE_ASSERT_DEBUG(offset.x + size.x <= mSize.x, return);
  CQDE_ASSERT_DEBUG(offset.y + size.y <= mSize.y, return);

  glTextureSubImage2D(
    mId, level,
    offset.x, offset.y,
    size.x, size.y,
    format, type, data );
}

void
GlTexture::generateMipmap() const
{
  CQDE_ASSERT_DEBUG(isValid() == true, return);

  glGenerateTextureMipmap(mId);
}

bool
GlTexture::isValid() const
{
  return mId != 0;
}

GLuint
GlTexture::id() const
{
  return mId;
}

glm::u16vec2
GlTexture::size() const
{
  return
  {
    mSize.x,
    mSize.y,
  };
}

GlTexture::SamplerParams
GlTexture::DefaultSamplerParams()
{
  return
  {
    {GL_TEXTURE_MIN_FILTER, GL_NEAREST},
    {GL_TEXTURE_MAG_FILTER, GL_NEAREST},
    {GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE},
    {GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE},
  };
}

} // namespace cqde::types
