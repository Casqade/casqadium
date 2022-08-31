#pragma once

#include <entt/fwd.hpp>

#include <json/forwards.h>


namespace cqde::compos
{

struct SubscriberInput
{
  SubscriberInput() = default;


  void ui_edit_props( const entt::entity,
                      const entt::registry& );

  Json::Value serialize() const;

  static void deserialize(
    entt::registry&,
    entt::entity,
    const Json::Value& );
};

} // namespace cqde::compos
