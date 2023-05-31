#pragma once

#include <cqde/types/enums.hpp>

#include <string>


namespace cqde::types
{

std::string TransformTypeToString( const TransformType );
TransformType TransformTypeFromString( const std::string& );

std::string MathFunctionTypeToString( const MathFunctionType );
MathFunctionType MathFunctionTypeFromString( const std::string& );

std::string ShaderTypeToString( const ShaderType );
ShaderType ShaderTypeFromString( const std::string& );

} // namespace cqde::types
