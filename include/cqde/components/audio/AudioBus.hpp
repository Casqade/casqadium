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

  AudioHandle handle() const;


  Json::Value serialize() const;

  static void deserialize(
    entt::registry&,
    entt::entity,
    const Json::Value&,
    const IdMap& = {} );
};

} // namespace cqde::compos
