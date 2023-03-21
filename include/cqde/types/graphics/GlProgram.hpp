#pragma once

#include <cqde/types/graphics/GlShader.hpp>

#include <glad/gl.h>

#include <map>


namespace cqde::types
{

class GlProgram
{
  std::map <const char*, GLint> mAttributes {};
  std::map <const char*, GLint> mUniforms {};

  GLuint mId {};

public:
  ~GlProgram();

  void create();
  void destroy();

  void attach( const GlShader& );
  void detach( const GlShader& );

  void link();

  void use() const;
  void unuse() const;

  GLint attribLocation( const char* name );
  GLint uniformLocation( const char* name );

  GLuint id() const;
};

} // namespace cqde::types
