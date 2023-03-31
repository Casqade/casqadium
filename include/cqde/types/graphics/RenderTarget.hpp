#pragma once

#include <cqde/types/graphics/GlTexture.hpp>

#include <glm/vec2.hpp>

#include <cstdint>


namespace cqde::types
{

struct RenderTarget
{
  using GLuint = uint32_t;

  glm::u16vec2 size {};

  GLuint fbo {};
  GLuint objectIds {};
  GLuint depthStencil {};

  GlTexture textureAlbedo {};


  RenderTarget();
  RenderTarget( const RenderTarget& );
  ~RenderTarget();

  RenderTarget& operator = ( const RenderTarget& );

  bool update( const glm::u16vec2& size );
  void destroy();
};

} // namespace cqde::types
