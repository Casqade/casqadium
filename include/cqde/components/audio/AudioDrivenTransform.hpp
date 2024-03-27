#pragma once

#include <cqde/alias.hpp>
#include <cqde/types/enums.hpp>
#include <cqde/types/EntityReference.hpp>

#include <entt/fwd.hpp>

#include <glm/vec3.hpp>

#include <json/forwards.h>


namespace cqde::compos
{

struct AudioDrivenTransform
{
  using TransformType = types::TransformType;
  using EntityReference = types::EntityReference;

  glm::vec3 axis {};
  float magnitude {1.0f};

  float amplitude {};
  float offset {};
  float damp {0.8f};

  TransformType operation {TransformType::Translate};
  bool useWorldSpace {};
  bool ignoreBusVolume {};

//  pick audio amplitude
//  from this entity
  EntityReference sourceEntity {}; // todo: use parent as source ?


  AudioDrivenTransform() = default;


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
