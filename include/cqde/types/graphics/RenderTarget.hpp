#pragma once

#include <cqde/types/graphics/GlTexture.hpp>

#include <glm/vec2.hpp>

#include <cstdint>


namespace cqde::types
{

struct RenderTarget
{
  glm::vec2 size {};

  uint32_t fbo {};
  uint32_t rbo {};

  GlTexture textureAlbedo {};
  GlTexture textureObjectIds {};


  RenderTarget();
  RenderTarget( const RenderTarget& );
  ~RenderTarget();

  RenderTarget& operator = ( const RenderTarget& );

  void update( const glm::vec2& size );
  void destroy();
};

} // namespace cqde::types
