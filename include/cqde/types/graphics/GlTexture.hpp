#pragma once

#include <glm/fwd.hpp>

#include <cstdint>
#include <cstddef>
#include <unordered_map>


namespace cqde::types
{

class GlTexture
{
  using GLint = int32_t;
  using GLuint = uint32_t;
  using GLenum = uint32_t;

  using SamplerParams = std::unordered_map <GLenum, GLint>;

  GLuint mId {};

  struct
  {
    uint16_t x {};
    uint16_t y {};

  } mSize {};

public:
  GlTexture() = default;

  void create(
    const glm::u16vec2& size,
    const GLenum type,
    const GLenum internalFormat,
    const size_t levels = 1,
    const SamplerParams& = DefaultSamplerParams() );

  void destroy();

  void store(
    const size_t level,
    const glm::u16vec2& offset,
    const glm::u16vec2& size,
    const GLenum format,
    const GLenum type,
    const void* data );

  void generateMipmap() const;

  bool isValid() const;

  GLuint id() const;
  glm::u16vec2 size() const;

  static SamplerParams DefaultSamplerParams();
};

} // namespace cqde::types
