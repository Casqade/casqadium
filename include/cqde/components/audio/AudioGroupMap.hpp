#pragma once

#include <cqde/alias.hpp>

#include <entt/fwd.hpp>

#include <json/forwards.h>

#include <unordered_map>
#include <vector>


namespace cqde::compos
{

struct AudioGroupMap
{
  using AudioGroup = std::vector <AudioId>;

  std::unordered_map <AudioGroupId, AudioGroup,
                      identifier_hash> groups {};


  AudioGroupMap() = default;


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
