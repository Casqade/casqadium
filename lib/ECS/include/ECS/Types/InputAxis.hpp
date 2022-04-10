#ifndef ECS_TYPES_INPUT_AXIS_HPP
#define ECS_TYPES_INPUT_AXIS_HPP

#include <ECS/Alias.hpp>

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

  std::set <InputCallbackId> callbacks;


  bool operator == ( const InputAxis& ) const;

  static std::string  toJson( const InputAxis& );
  static InputAxis    fromJson( const std::string& );
};

} // namespace Types

} // namespace ECS

#endif
