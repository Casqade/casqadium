#pragma once

#include <cqde/alias.hpp>
#include <cqde/common.hpp>

#include <set>
#include <string>


namespace cqde::types
{

struct ControlAxis
{
  float value;
  std::pair <float, float> constraint;

  std::set <InputCallbackId> callbacks;


  ControlAxis() = default;

  bool operator == ( const ControlAxis& ) const;

  static std::string  toJson( const ControlAxis& );
  static ControlAxis  fromJson( const std::string& );
};

} // namespace cqde::types
