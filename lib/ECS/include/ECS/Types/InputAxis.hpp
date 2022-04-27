#pragma once

#include <ECS/Alias.hpp>
#include <ECS/Common.hpp>

#include <set>
#include <string>


namespace ECS
{

namespace Types
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

} // namespace Types

} // namespace ECS
