#pragma once

#include <cqde/alias.hpp>
#include <cqde/types/EntityReference.hpp>

#include <entt/fwd.hpp>

#include <json/forwards.h>


namespace cqde::compos
{

struct InteractionProbe
{
  using EntityReference = types::EntityReference;

  EntityReference listener {};


  InteractionProbe() = default;


  void ui_edit_props( const entt::entity,
                      const entt::registry& );

  Json::Value serialize() const;

  static void deserialize(
    entt::registry&,
    entt::entity,
    const Json::Value&,
    const std::unordered_map <EntityId, EntityId,
                              identifier_hash>& idMap = {} );
};

} // namespace cqde::compos
