#include <cqde/types/input/ControlAxis.hpp>

#include <entt/entt.hpp>

#include <json/value.h>


namespace cqde::types
{

bool
ControlAxis::operator == ( const ControlAxis& other ) const
{
  return  std::tie(value, constraint, callbacks) ==
          std::tie(other.value, other.constraint, other.callbacks);
}

Json::Value
ControlAxis::toJson( const ControlAxis& axis )
{
  return {};
}

ControlAxis
ControlAxis::fromJson( const Json::Value& json )
{
  return {};
}

} // namespace cqde::types
