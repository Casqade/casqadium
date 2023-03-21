#pragma once

#include <entt/fwd.hpp>

#include <json/forwards.h>


namespace cqde::compos
{

struct ComponentTemplate
{
  ComponentTemplate() = default;


  Json::Value serialize() const;

  void deserialize( entt::registry&,
                    entt::entity,
                    const Json::Value& ) const;

  static void Register();
};

} // namespace cqde::compos
