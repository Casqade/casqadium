#pragma once

#include <cqde/alias.hpp>

#include <entt/fwd.hpp>

#include <json/forwards.h>


namespace cqde::compos
{

struct Tag
{
  EntityId id {};


  Tag() = default;

  void invalidate( entt::registry& );


  Json::Value serialize() const;

  void deserialize( entt::registry&,
                    entt::entity,
                    const Json::Value& ) const;
};

} // namespace cqde::compos
