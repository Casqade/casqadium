#pragma once

#include <TimeUtils/Duration.hpp>

#include <entt/fwd.hpp>

#include <json/forwards.h>


namespace cqde::types
{

struct TimeProgress
{
  using Duration = TimeUtils::Duration;

  Duration elapsed {};
  Duration total {};


  TimeProgress() = default;

  bool expired( entt::registry& );

  double progress() const;

  void ui_show();

  Json::Value toJson() const;
  void fromJson( const Json::Value& );
};

} // namespace cqde::types

