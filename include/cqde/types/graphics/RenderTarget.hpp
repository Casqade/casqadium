#pragma once

#include <glm/vec2.hpp>

#include <cstdint>


namespace cqde::types
{

struct RenderTarget
{
  glm::vec2 size {};

  uint32_t fbo {};
  uint32_t rbo {};

  uint32_t textureAlbedo {};
  uint32_t textureObjectIds {};


  RenderTarget();
  RenderTarget( const RenderTarget& );
  ~RenderTarget();

  void update( const glm::vec2& size );
  void destroy();
};

} // namespace cqde::types
