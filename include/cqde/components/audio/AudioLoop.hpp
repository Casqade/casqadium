#pragma once

#include <cqde/alias.hpp>

#include <entt/fwd.hpp>

#include <json/forwards.h>

#include <vector>


namespace cqde::compos
{

struct AudioLoop
{
  struct AudioLayer
  {
    AudioId source {};
    AudioHandle handle {AudioHandleInvalid};
  };

  std::vector <AudioLayer> layers {};
  AudioHandle handle {};


  AudioLoop() = default;


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
