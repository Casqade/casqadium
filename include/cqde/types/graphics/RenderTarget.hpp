#pragma once

#include <cqde/types/graphics/GlTexture.hpp>

#include <glm/vec2.hpp>

#include <cstdint>


namespace cqde::types
{

struct RenderTarget
{
  glm::u16vec2 size {};

  uint32_t fbo {};
  uint32_t objectIds {};
  uint32_t depthStencil {};

  GlTexture textureAlbedo {};


  RenderTarget();
  RenderTarget( const RenderTarget& );
  ~RenderTarget();

  RenderTarget& operator = ( const RenderTarget& );

  void update( const glm::u16vec2& size );
  void destroy();
};

} // namespace cqde::types
