#pragma once

#include <cqde/alias.hpp>

#include <entt/fwd.hpp>

#include <json/forwards.h>


namespace cqde::compos
{

struct CasqadiumEditorInternal
{
  CasqadiumEditorInternal() = default;


  Json::Value serialize() const;

  static void deserialize(
    entt::registry&,
    entt::entity,
    const Json::Value&,
    const IdMap& = {} );
};

} // namespace cqde::compos
