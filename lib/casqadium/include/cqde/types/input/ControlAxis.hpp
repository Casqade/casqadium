#pragma once

#include <cqde/alias.hpp>

#include <json/forwards.h>

#include <set>


namespace cqde::types
{

struct ControlAxis
{
  float value {};
  std::pair <float, float> constraint {};
//          second < first to disable limits

  std::set <CallbackId> callbacks {};


  ControlAxis() = default;
  ControlAxis( const Json::Value& );

  bool operator == ( const ControlAxis& ) const;

  Json::Value toJson() const;
};

} // namespace cqde::types
