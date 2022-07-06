#pragma once

#include <entt/fwd.hpp>

#include <json/forwards.h>


namespace cqde::compos
{

struct EntityMetaInfo
{
  entt::id_type packageId {};


  EntityMetaInfo() = default;


  Json::Value serialize() const;

  static void deserialize(
    entt::registry&,
    entt::entity,
    const Json::Value& );
};

} // namespace cqde::compos
