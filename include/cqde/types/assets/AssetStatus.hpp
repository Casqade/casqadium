#pragma once

#include <string>
#include <cstdint>


namespace cqde::types
{

enum class AssetStatus
{
  Undefined,
  Unloaded,
  Loading,
  Loaded,
  Error,
};


uint32_t AssetStatusAsColor( const AssetStatus );
std::string AssetStatusAsString( const AssetStatus );

} // namespace cqde::types
