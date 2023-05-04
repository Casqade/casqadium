#pragma once

#include <string>
#include <cstdint>


namespace cqde::types
{

class GlShader
{
  using GLenum = uint32_t;
  using GLuint = uint32_t;

  GLuint mId {};

public:
  GlShader() = default;
  ~GlShader();

  void load( const char*, const GLenum type );
  void load( const std::string& path, const GLenum type );
  void unload();

  bool isValid() const;

  GLuint id() const;
};

} // namespace cqde::types
