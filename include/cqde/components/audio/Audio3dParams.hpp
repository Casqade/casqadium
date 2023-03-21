#pragma once

#include <cqde/alias.hpp>

#include <entt/fwd.hpp>

#include <json/forwards.h>

#include <soloud.h>


namespace cqde::compos
{

struct Audio3dParams
{
  using AttenuationModel = SoLoud::AudioSource::ATTENUATION_MODELS;

  struct
  {
    AttenuationModel model {};
    float rolloff {1.0f};

    struct
    {
      float min {1.0f};
      float max {1000000.0f};

    } distance {};

  } attenuation {};

  float dopplerFactor {1.0f};


  Audio3dParams() = default;


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
