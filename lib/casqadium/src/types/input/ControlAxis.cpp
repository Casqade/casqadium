#include <cqde/types/input/ControlAxis.hpp>


namespace cqde::types
{

bool
ControlAxis::operator == ( const ControlAxis& other ) const
{
  return  std::tie(value, constraint, callbacks) ==
          std::tie(other.value, other.constraint, other.callbacks);
}

std::string
ControlAxis::toJson( const ControlAxis& )
{
  return {};
}

ControlAxis
ControlAxis::fromJson( const std::string& json )
{
  return {};
}

} // namespace cqde::types
