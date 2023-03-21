#include <cqde/types/graphics/RenderTarget.hpp>

#include <cqde/common.hpp>

#include <glad/gl.h>

#include <unordered_map>


namespace cqde::types
{

struct GlTexture
{
  using SamplerParams = std::unordered_map <GLenum, GLint>;

  uint32_t mId {};


  void create(
    GLenum type,
    const glm::vec2& size,
    const SamplerParams& = DefaultSamplerParams() );

  void destroy();

  static SamplerParams DefaultSamplerParams();
};

RenderTarget::RenderTarget()
{
  update({1.0f, 1.0f});
}

RenderTarget::RenderTarget(
  const RenderTarget& other )
{
  update(other.size);
}

RenderTarget::~RenderTarget()
{
  destroy();
}

void
RenderTarget::update(
  const glm::vec2& size )
{
  if ( this->size == size )
    return;

  this->size = size;

  destroy();

  glCreateFramebuffers(1, &fbo);
  glCreateRenderbuffers(1, &rbo);

  const auto updateTexture =
  [this, size]
  ( GLuint& texture,
    const GLenum internalFormat,
    const GLenum attachment )
  {
    if ( texture != 0 )
      glDeleteTextures(1, &texture);

    glCreateTextures(GL_TEXTURE_2D, 1, &texture);

    glTextureParameteri( texture,
      GL_TEXTURE_MIN_FILTER, GL_NEAREST );

    glTextureParameteri( texture,
      GL_TEXTURE_MAG_FILTER, GL_NEAREST );

    glTextureParameteri( texture,
      GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );

    glTextureParameteri( texture,
      GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

    glTextureStorage2D( texture, 1,
      internalFormat,
      size.x, size.y );

    glNamedFramebufferTexture( fbo,
      attachment,
      texture, 0 );
  };

  updateTexture(textureAlbedo, GL_RGBA8, GL_COLOR_ATTACHMENT0);
  glGenerateTextureMipmap(textureAlbedo);
  updateTexture(textureObjectIds, GL_R32UI, GL_COLOR_ATTACHMENT1);

  glNamedRenderbufferStorageMultisample( rbo,
    0, GL_DEPTH24_STENCIL8,
    size.x, size.y );

  glNamedFramebufferRenderbuffer( fbo,
    GL_DEPTH_STENCIL_ATTACHMENT,
    GL_RENDERBUFFER, rbo );

  const GLuint attachements[]
  {
    GL_COLOR_ATTACHMENT0,
    GL_COLOR_ATTACHMENT1,
  };

  glNamedFramebufferDrawBuffers(fbo, sizeof(attachements) / sizeof(attachements[0]), attachements);

  const auto status = glCheckNamedFramebufferStatus(fbo, GL_FRAMEBUFFER);
  CQDE_ASSERT_DEBUG(status == GL_FRAMEBUFFER_COMPLETE, return);
}

void
RenderTarget::destroy()
{
  if ( rbo != 0 )
    glDeleteRenderbuffers(1, &rbo);

  if ( textureAlbedo != 0 )
    glDeleteTextures(1, &textureAlbedo);

  if ( fbo != 0 )
    glDeleteFramebuffers(1, &fbo);

  fbo = rbo = textureAlbedo = 0;
}

} // namespace cqde::types
