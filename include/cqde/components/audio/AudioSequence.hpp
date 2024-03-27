#pragma once

#include <cqde/alias.hpp>
#include <cqde/types/audio/SoloudQueue.hpp>

#include <entt/fwd.hpp>

#include <json/forwards.h>

#include <memory>


namespace cqde::compos
{

struct AudioSequence
{
  using SoloudQueue = types::SoloudQueue;

  std::shared_ptr <SoloudQueue> queue {};


  AudioSequence();
  AudioSequence( AudioSequence&& );
  AudioSequence& operator = ( AudioSequence&& );

  ~AudioSequence();


  Json::Value serialize() const;

  static void deserialize(
    entt::registry&,
    entt::entity,
    const Json::Value&,
    const IdMap& = {} );
};

} // namespace cqde::compos
