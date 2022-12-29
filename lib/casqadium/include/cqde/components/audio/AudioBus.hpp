#pragma once

#include <cqde/alias.hpp>

#include <entt/fwd.hpp>

#include <json/forwards.h>

#include <soloud_bus.h>

#include <memory>


namespace cqde::compos
{

struct AudioBus
{
  std::unique_ptr <SoLoud::Bus> bus {};


  AudioBus();
  AudioBus( AudioBus&& );
  AudioBus& operator = ( AudioBus&& );

  ~AudioBus();

  void ensureIsPlaying( SoLoud::Soloud& );

  Json::Value serialize() const;

  static void deserialize(
    entt::registry&,
    entt::entity,
    const Json::Value&,
    const std::unordered_map <EntityId, EntityId,
                              identifier_hash>& idMap = {} );
};

} // namespace cqde::compos
