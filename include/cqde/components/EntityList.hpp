#pragma once

#include <cqde/alias.hpp>
#include <cqde/types/EntityReference.hpp>

#include <entt/fwd.hpp>

#include <json/forwards.h>

#include <unordered_set>


namespace cqde::compos
{

struct EntityList
{
  using EntityReference = types::EntityReference;

  std::unordered_set <EntityReference, identifier_hash> entities {};


  EntityList() = default;


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
