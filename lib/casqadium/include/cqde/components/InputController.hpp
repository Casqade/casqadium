#pragma once

#include <cqde/alias.hpp>
#include <cqde/common.hpp>
#include <cqde/types/input/ControlAxis.hpp>

#include <entt/fwd.hpp>

#include <json/forwards.h>

#include <map>


namespace cqde::compos
{

struct InputController
{
  std::map <InputAxisId, types::ControlAxis> inputs;


  Json::Value serialize() const;

  static void deserialize(
    entt::registry&,
    entt::entity,
    const Json::Value& );
};

} // namespace cqde::compos
