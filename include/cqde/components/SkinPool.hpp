#pragma once

#include <cqde/alias.hpp>

#include <entt/fwd.hpp>

#include <json/forwards.h>


namespace cqde::compos
{

struct SkinPool
{
  std::unordered_map <MeshId, std::vector <TextureId>,
                      identifier_hash> skins {};


  SkinPool() = default;


  void ui_edit_props(
    const entt::entity,
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
