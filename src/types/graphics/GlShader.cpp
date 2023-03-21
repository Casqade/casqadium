#include <cqde/types/graphics/GlShader.hpp>

#include <cqde/common.hpp>
#include <cqde/file_helpers.hpp>
#include <cqde/logger.hpp>

#include <spdlog/fmt/bundled/format.h>

#include <fstream>


namespace cqde::types
{

GlShader::~GlShader()
{
  unload();
}

void
GlShader::load(
  const char* code,
  const GLenum type )
{
  using fmt::format;

  mId = glCreateShader(type);
  glShaderSource(mId, 1, &code, nullptr);
  glCompileShader(mId);

  GLint status {};
  glGetShaderiv(mId, GL_COMPILE_STATUS, &status);

  GLint infoLogLength = 0;
  glGetShaderiv(mId, GL_INFO_LOG_LENGTH, &infoLogLength);

  std::vector <GLchar> infoLogBuffer(infoLogLength);

  glGetShaderInfoLog(mId, infoLogLength, nullptr,
                     infoLogBuffer.data());

  if (  status == true )
  {
    if ( infoLogBuffer.empty() == false )
      LOG_DEBUG("Shader compilation log: '{}'",
                infoLogBuffer.data());
    return;
  }


  unload();

  throw std::runtime_error(format(
    "Compilation error: {}",
    infoLogBuffer.data() ));
}

void
GlShader::load(
  const std::string& path,
  const GLenum type )
{
  using fmt::format;
  using cqde::fileOpen;

  auto shaderFile = fileOpen(path, std::ios::in);

  std::stringstream shaderStream {};
  shaderStream << shaderFile.rdbuf();
  shaderFile.close();

  const auto shaderString = shaderStream.str();
  const auto shaderCString = shaderString.data();

  mId = glCreateShader(type);
  glShaderSource(mId, 1, &shaderCString, nullptr);
  glCompileShader(mId);

  GLint status {};
  glGetShaderiv(mId, GL_COMPILE_STATUS, &status);

  GLint infoLogLength = 0;
  glGetShaderiv(mId, GL_INFO_LOG_LENGTH, &infoLogLength);

  std::vector <GLchar> infoLogBuffer(infoLogLength);

  glGetShaderInfoLog(mId, infoLogLength, nullptr,
                     infoLogBuffer.data());

  if ( status == true )
  {
    if ( infoLogBuffer.empty() == false )
      LOG_DEBUG("Shader compilation log: '{}'",
                infoLogBuffer.data());
    return;
  }


  unload();

  throw std::runtime_error(format(
    "Compilation error: {}",
    infoLogBuffer.data() ));
}

void
GlShader::unload()
{
  glDeleteShader(mId);
  mId = 0;
}

GLuint
GlShader::id() const
{
  return mId;
}

} // namespace cqde::types
