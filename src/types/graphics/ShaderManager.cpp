#include <cqde/types/graphics/ShaderManager.hpp>

#include <cqde/common.hpp>
#include <cqde/logger.hpp>

#include <glad/gl.h>


namespace cqde::types
{

bool
ShaderManager::load(
  const ShaderType type,
  const char* vertexCode,
  const char* fragmentCode )
{
  GlShader shaderVertex {};
  GlShader shaderFragment {};

  try
  {
    shaderVertex.load(vertexCode, GL_VERTEX_SHADER);
  }
  catch ( const std::exception& e )
  {
    LOG_ERROR("Failed to load '{}' vertex shader from memory: {}",
              ShaderTypeToString(type), e.what());
    return false;
  }

  try
  {
    shaderFragment.load(fragmentCode, GL_FRAGMENT_SHADER);
  }
  catch ( const std::exception& e )
  {
    LOG_ERROR("Failed to load '{}' fragment shader from memory: {}",
              ShaderTypeToString(type), e.what());
    return false;
  }

  auto& program = mShaders[type];

  try
  {
    program.destroy();
    program.create();
    program.attach(shaderVertex);
    program.attach(shaderFragment);
    program.link();
  }
  catch ( const std::exception& e )
  {
    LOG_ERROR(e.what());
    return false;
  }

  return true;
}

bool
ShaderManager::load(
  const ShaderType type,
  const std::string& vertexPath,
  const std::string& fragmentPath )
{
  GlShader shaderVertex {};
  GlShader shaderFragment {};

  try
  {
    shaderVertex.load(vertexPath, GL_VERTEX_SHADER);
  }
  catch ( const std::exception& e )
  {
    LOG_ERROR("Failed to load vertex shader '{}': {}",
              vertexPath, e.what());
    return false;
  }

  try
  {
    shaderFragment.load(fragmentPath, GL_FRAGMENT_SHADER);
  }
  catch ( const std::exception& e )
  {
    LOG_ERROR("Failed to load fragment shader '{}': {}",
              fragmentPath, e.what());
    return false;
  }

  auto& program = mShaders[type];

  try
  {
    program.destroy();
    program.create();
    program.attach(shaderVertex);
    program.attach(shaderFragment);
    program.link();
  }
  catch ( const std::exception& e )
  {
    LOG_ERROR(e.what());
    return false;
  }

  return true;
}

GlProgram&
ShaderManager::get(
  const ShaderType type )
{
  return mShaders.at(type);
}

std::string
ShaderManager::ShaderTypeToString(
  const ShaderType type )
{
  switch (type)
  {
    case ShaderType::Geometry:
      return "Geometry";

    case ShaderType::Shadows:
      return "Shadows";

    case ShaderType::UiElements:
      return "UiElements";
  }

  CQDE_ASSERT_DEBUG(false, return "");
}

ShaderType
ShaderManager::ShaderTypeFromString(
  const std::string& type )
{
  using fmt::format;

  if ( type == ShaderTypeToString(ShaderType::Geometry) )
    return ShaderType::Geometry;

  if ( type == ShaderTypeToString(ShaderType::Shadows) )
    return ShaderType::Shadows;

  if ( type == ShaderTypeToString(ShaderType::UiElements) )
    return ShaderType::UiElements;

  throw std::runtime_error(
    format("'{}' is not a valid shader type", type));
}

} // namespace cqde::types
