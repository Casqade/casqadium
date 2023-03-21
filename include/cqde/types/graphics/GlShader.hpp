#pragma once

#include <glad/gl.h>

#include <string>


namespace cqde::types
{

class GlShader
{
  GLuint mId {};

public:
  GlShader() = default;
  ~GlShader();

  void load( const char*, const GLenum type );
  void load( const std::string& path, const GLenum type );
  void unload();

  GLuint id() const;
};

} // namespace cqde::types
