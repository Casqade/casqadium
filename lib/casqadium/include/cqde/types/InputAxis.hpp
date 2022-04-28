#pragma once

#include <cqde/alias.hpp>
#include <cqde/common.hpp>

#include <set>
#include <string>


namespace cqde::types
{

struct InputAxis
{
  float value;
  float sensitivity;
  std::pair <float, float> constraint;

  std::set <InputCallbackId,
            entt::hashed_string_comparator> callbacks;


  bool operator == ( const InputAxis& ) const;

  static std::string  toJson( const InputAxis& );
  static InputAxis    fromJson( const std::string& );
};

} // namespace cqde::types
