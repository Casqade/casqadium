#include <cqde/types/InputAxis.hpp>


namespace cqde::types
{

bool
InputAxis::operator == ( const InputAxis& other ) const
{
  return
  (
    value == other.value &&
    sensitivity == other.sensitivity &&
    constraint == other.constraint &&
    callbacks == other.callbacks
  );
}

std::string
InputAxis::toJson( const InputAxis& )
{
  return {};
}

InputAxis
InputAxis::fromJson( const std::string& json )
{
  return {};
}

} // namespace cqde::types
