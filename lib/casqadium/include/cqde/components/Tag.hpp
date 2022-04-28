#pragma once

#include <cqde/alias.hpp>

#include <entt/fwd.hpp>

#include <json/forwards.h>


namespace cqde::compos
{

struct Tag
{
  EntityId id;


  static Tag Generate( entt::registry&, entt::hashed_string = {} );


  void serialize( Json::Value& ) const;

  void deserialize( entt::registry&,
                    entt::entity,
                    const Json::Value& ) const;

  static void Register();
};

} // namespace cqde::compos
