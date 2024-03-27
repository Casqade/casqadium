#pragma once

#include <cqde/alias.hpp>

#include <cqde/types/sequences/SequenceStep.hpp>

#include <entt/fwd.hpp>

#include <json/forwards.h>

#include <deque>


namespace cqde::compos
{

struct SequenceController
{
  using SequenceStep = types::SequenceStep;

  std::deque <std::shared_ptr <SequenceStep>> steps {};


  SequenceController() = default;


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
