#pragma once

#include <cqde/alias.hpp>

#include <entt/fwd.hpp>

#include <json/forwards.h>

#include <unordered_set>


namespace cqde::compos
{

struct SystemList
{
  std::unordered_set <SystemId, identifier_hash> systems {};


  SystemList() = default;


  void ui_edit_props( const entt::entity,
                      const entt::registry& );

  Json::Value serialize() const;

  static void deserialize(
    entt::registry&,
    entt::entity,
    const Json::Value&,
    const IdMap& = {} );
};

} // namespace cqde::compos
