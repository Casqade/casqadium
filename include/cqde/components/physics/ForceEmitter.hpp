#pragma once

#include <cqde/alias.hpp>

#include <entt/fwd.hpp>

#include <glm/vec4.hpp>

#include <json/forwards.h>


namespace cqde::compos
{

struct ForceEmitter
{
//  {vec3 dir, float amount}
//  null direction points to emitter position
  glm::vec4 force {};
  bool useWorldSpace {};


  ForceEmitter() = default;


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
