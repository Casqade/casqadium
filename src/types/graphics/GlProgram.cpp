#include <cqde/types/graphics/GlProgram.hpp>

#include <cqde/assert.hpp>
#include <cqde/file_helpers.hpp>

#include <glad/gl.h>

#include <spdlog/fmt/bundled/format.h>

#include <vector>


namespace cqde::types
{

GlProgram::~GlProgram()
{
  destroy();
}

void
GlProgram::create()
{
  CQDE_ASSERT_DEBUG(isValid() == false, destroy());

  mId = glCreateProgram();

  CQDE_ASSERT_DEBUG(isValid() == true, return);
}

void
GlProgram::destroy()
{
  mAttributes.clear();
  glDeleteProgram(mId);
  mId = 0;
}

void
GlProgram::attach(
  const GlShader& shader )
{
  CQDE_ASSERT_DEBUG(isValid() == true, return);
  CQDE_ASSERT_DEBUG(shader.isValid() == true, return);

  glAttachShader(mId, shader.id());
}

void
GlProgram::detach(
  const GlShader& shader )
{
  CQDE_ASSERT_DEBUG(isValid() == true, return);
  CQDE_ASSERT_DEBUG(shader.isValid() == true, return);

  glDetachShader(mId, shader.id());
}

void
GlProgram::link()
{
  using fmt::format;

  CQDE_ASSERT_DEBUG(isValid() == true, return);

  mAttributes.clear();

  glLinkProgram(mId);

  GLint status {};
  glGetProgramiv(mId, GL_LINK_STATUS, &status);

  GLint infoLogLength = 0;
  glGetProgramiv(mId, GL_INFO_LOG_LENGTH, &infoLogLength);

  std::vector <GLchar> infoLogBuffer(infoLogLength);
  glGetProgramInfoLog(mId, infoLogLength, nullptr,
                      infoLogBuffer.data());

  if ( status == true )
    return;


  destroy();

  throw std::runtime_error(format(
    "Shader program linking error: {}",
    infoLogBuffer.data() ));
}

void
GlProgram::use() const
{
  CQDE_ASSERT_DEBUG(isValid() == true, return);

  glUseProgram(mId);
}

void
GlProgram::unuse() const
{
  CQDE_ASSERT_DEBUG(isValid() == true, return);

  glUseProgram(0);
}

GLint
GlProgram::attribLocation(
  const char* name )
{
  CQDE_ASSERT_DEBUG(isValid() == true, return -1);

  const auto iter = mAttributes.find(name);

  if ( iter != mAttributes.end() )
    return iter->second;

  return mAttributes[name] = glGetAttribLocation(mId, name);
}

GLint
GlProgram::uniformLocation(
  const char* name )
{
  CQDE_ASSERT_DEBUG(isValid() == true, return -1);

  const auto iter = mUniforms.find(name);

  if ( iter != mUniforms.end() )
    return iter->second;

  return mUniforms[name] = glGetUniformLocation(mId, name);
}

bool
GlProgram::isValid() const
{
  return mId != 0;
}

GLuint
GlProgram::id() const
{
  return mId;
}

} // namespace cqde::types
