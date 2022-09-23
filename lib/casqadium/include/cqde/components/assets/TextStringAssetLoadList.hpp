#pragma once

#include <cqde/alias.hpp>

#include <entt/fwd.hpp>

#include <json/forwards.h>

#include <unordered_set>


namespace cqde::compos
{

struct TextStringAssetLoadList
{
  std::unordered_set <StringId, identifier_hash> textToLoad {};


  TextStringAssetLoadList() = default;


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
