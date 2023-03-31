#pragma once

#include <cqde/types/graphics/GlVertexArray.hpp>
#include <cqde/types/graphics/RenderTarget.hpp>

#include <glad/gl.h>


namespace cqde::types
{

struct PrimaryRenderTarget
{
  using GlBuffer = types::GlBuffer;
  using GlVertexArray = types::GlVertexArray;
  using RenderTarget = types::RenderTarget;


  RenderTarget target {};

  GlVertexArray vao {};
  GlBuffer vertices {};
  GlBuffer texCoords {};


  PrimaryRenderTarget();
  ~PrimaryRenderTarget();
};

} // namespace cqde::types
