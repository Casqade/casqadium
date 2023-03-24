#pragma once

#include <cqde/types/enums.hpp>
#include <cqde/types/graphics/GlProgram.hpp>

#include <unordered_map>


namespace cqde::types
{

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
  const GlProgram& get( const ShaderType ) const;
};

} // namespace cqde::types
