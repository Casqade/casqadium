#pragma once

#include <cqde/alias.hpp>
#include <cqde/types/input/ControlAxis.hpp>

#include <entt/fwd.hpp>

#include <json/forwards.h>

#include <map>


namespace cqde::compos
{

struct InputController
{
  std::unordered_map <InputAxisId, types::ControlAxis,
                      identifier_hash> axes {};


  InputController() = default;


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
