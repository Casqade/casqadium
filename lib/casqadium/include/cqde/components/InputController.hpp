#pragma once

#include <cqde/alias.hpp>
#include <cqde/common.hpp>
#include <cqde/types/InputAxis.hpp>

#include <entt/fwd.hpp>

#include <map>
#include <string>


namespace cqde::compos
{

struct InputController
{
  std::map <InputDestinationId, types::InputAxis,
            entt::hashed_string_comparator> inputs;


  void serialize( const std::string& json ) const;

  void deserialize( entt::registry&,
                    entt::entity,
                    const std::string& json ) const;

  static void Register();
};

} // namespace cqde::compos
