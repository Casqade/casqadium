#pragma once

#include <cqde/alias.hpp>
#include <cqde/common.hpp>
#include <cqde/types/InputAxis.hpp>

#include <entt/fwd.hpp>

#include <json/forwards.h>

#include <map>


namespace cqde::compos
{

struct InputController
{
  std::map <InputDestinationId, types::InputAxis,
            entt::hashed_string_comparator> inputs;


  void serialize( Json::Value& ) const;

  void deserialize( entt::registry&,
                    entt::entity,
                    const Json::Value& ) const;

  static void Register();
};

} // namespace cqde::compos
