#pragma once

#include <entt/fwd.hpp>

#include <json/forwards.h>


namespace cqde::compos
{

struct ComponentTemplate
{
  ComponentTemplate() = default;


  void ui_edit_props( const entt::entity,
                      const entt::registry& );

  Json::Value serialize() const;

  void deserialize(
    entt::registry&,
    entt::entity,
    const Json::Value&,
    const IdMap& = {} );
};

} // namespace cqde::compos
