#pragma once

#include <cqde/alias.hpp>

#include <entt/fwd.hpp>

#include <glm/vec2.hpp>

#include <json/forwards.h>


namespace cqde::compos
{

struct alignas(64) DrawableMesh
{
  MeshId meshId {};
  TextureId textureId {};


  DrawableMesh() = default;


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
