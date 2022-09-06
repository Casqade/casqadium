#pragma once

#include <cqde/alias.hpp>

#include <entt/fwd.hpp>

#include <json/forwards.h>

#include <deque>


namespace cqde::compos
{

struct SequenceController
{
  std::deque <CallbackId> steps {};


  SequenceController() = default;


  void ui_edit_props( const entt::entity,
                      const entt::registry& );

  Json::Value serialize() const;

  static void deserialize(
    entt::registry&,
    entt::entity,
    const Json::Value& );
};

} // namespace cqde::compos
