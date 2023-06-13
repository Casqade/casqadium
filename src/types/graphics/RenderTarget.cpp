#include <cqde/types/graphics/RenderTarget.hpp>

#include <cqde/assert.hpp>

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

bool
RenderTarget::update(
  const glm::u16vec2& size )
{
  if ( this->size == size &&
       isValid() == true )
    return false;

  CQDE_ASSERT_DEBUG(size.x > 0 && size.y > 0, return false);

  this->size = size;

  destroy();

  glCreateFramebuffers(1, &fbo);
  glCreateRenderbuffers(1, &objectIds);
  glCreateRenderbuffers(1, &depthStencil);

  textureAlbedo.create(size, GL_TEXTURE_2D, GL_RGBA8);

  textureAlbedo.generateMipmap();

  glNamedFramebufferTexture( fbo,
    GL_COLOR_ATTACHMENT0,
    textureAlbedo.id(), 0 );

  glNamedRenderbufferStorageMultisample( objectIds,
    0, GL_R32UI,
    size.x, size.y );

  glNamedRenderbufferStorageMultisample( depthStencil,
    0, GL_DEPTH24_STENCIL8,
    size.x, size.y );

  glNamedFramebufferRenderbuffer( fbo,
    GL_COLOR_ATTACHMENT1,
    GL_RENDERBUFFER, objectIds );

  glNamedFramebufferRenderbuffer( fbo,
    GL_DEPTH_STENCIL_ATTACHMENT,
    GL_RENDERBUFFER, depthStencil );

  const GLuint attachements[]
  {
    GL_COLOR_ATTACHMENT0,
    GL_COLOR_ATTACHMENT1,
  };

  glNamedFramebufferDrawBuffers(fbo, sizeof(attachements) / sizeof(attachements[0]), attachements);

  const auto status = glCheckNamedFramebufferStatus(fbo, GL_FRAMEBUFFER);
  CQDE_ASSERT_DEBUG(status == GL_FRAMEBUFFER_COMPLETE, return false);

  return true;
}

void
RenderTarget::destroy()
{
  if ( depthStencil != 0 )
    glDeleteRenderbuffers(1, &depthStencil);

  if ( objectIds != 0 )
    glDeleteRenderbuffers(1, &objectIds);

  if ( textureAlbedo.isValid() == true )
    textureAlbedo.destroy();

  if ( fbo != 0 )
    glDeleteFramebuffers(1, &fbo);

  fbo = depthStencil = objectIds = 0;
}

bool
RenderTarget::isValid() const
{
  return
    fbo != 0 &&
    objectIds != 0 &&
    depthStencil != 0 &&
    textureAlbedo.isValid() == true;
}

} // namespace cqde::types
