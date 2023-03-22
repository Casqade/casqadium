#include <cqde/types/graphics/RenderTarget.hpp>

#include <cqde/common.hpp>

#include <glad/gl.h>


namespace cqde::types
{

RenderTarget::RenderTarget()
{
  update({1.0f, 1.0f});
}

RenderTarget::RenderTarget(
  const RenderTarget& other )
{
  *this = other;
}

RenderTarget::~RenderTarget()
{
  destroy();
}

RenderTarget&
RenderTarget::operator = (
  const RenderTarget& other )
{
  size = other.size;

  update(size);

  return *this;
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

  textureAlbedo.create(size, GL_TEXTURE_2D, GL_RGBA8);
  textureObjectIds.create(size, GL_TEXTURE_2D, GL_R32UI);

  textureAlbedo.generateMipmap();

  glNamedFramebufferTexture( fbo,
    GL_COLOR_ATTACHMENT0,
    textureAlbedo.id(), 0 );

  glNamedFramebufferTexture( fbo,
    GL_COLOR_ATTACHMENT1,
    textureObjectIds.id(), 0 );

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

  if ( textureAlbedo.isValid() == true )
    textureAlbedo.destroy();

  if ( textureObjectIds.isValid() == true )
    textureObjectIds.destroy();

  if ( fbo != 0 )
    glDeleteFramebuffers(1, &fbo);

  fbo = rbo = 0;
}

} // namespace cqde::types
