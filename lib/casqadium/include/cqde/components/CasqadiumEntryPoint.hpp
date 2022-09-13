#pragma once

#include <cqde/alias.hpp>

#include <entt/fwd.hpp>

#include <json/forwards.h>

#include <unordered_set>


namespace cqde::compos
{

struct CasqadiumEntryPoint
{
  std::unordered_set <SystemId, identifier_hash> systemsToEnable {};
  std::unordered_set <EntityId, identifier_hash> entitiesToEnableUpdate {};
  std::unordered_set <EntityId, identifier_hash> entitiesToEnableInput {};


  CasqadiumEntryPoint() = default;


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
