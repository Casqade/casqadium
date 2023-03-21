#pragma once

#include <cqde/types/graphics/GlProgram.hpp>

#include <unordered_map>


namespace cqde::types
{

enum class ShaderType
{
  Geometry,
  Shadows,
  UiElements,
};

class ShaderManager
{
private:
  std::unordered_map <ShaderType, GlProgram> mShaders {};

public:
  ShaderManager() = default;

  bool load(
    const ShaderType,
    const char* vertexCode,
    const char* fragmentCode );

  bool load(
    const ShaderType,
    const std::string& vertexPath,
    const std::string& fragmentPath );

  GlProgram& get( const ShaderType );


  static std::string ShaderTypeToString( const ShaderType );
  static ShaderType ShaderTypeFromString( const std::string& );
};

} // namespace cqde::types
