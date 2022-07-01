#include <cqde/types/input/ControlAxis.hpp>

#include <entt/entt.hpp>

#include <json/value.h>


namespace cqde::types
{

ControlAxis::ControlAxis( const Json::Value& json )
{}

bool
ControlAxis::operator == ( const ControlAxis& other ) const
{
  return  std::tie(value, constraint, callbacks) ==
          std::tie(other.value, other.constraint, other.callbacks);
}

Json::Value
ControlAxis::toJson() const
{
  return {};
}

} // namespace cqde::types
