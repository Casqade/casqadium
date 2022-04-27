#pragma once

#include <ECS/Alias.hpp>
#include <ECS/Common.hpp>
#include <ECS/Types/InputAxis.hpp>

#include <entt/fwd.hpp>

#include <map>
#include <string>


namespace ECS
{

namespace Components
{

struct InputController
{
  std::map <InputDestinationId, Types::InputAxis,
            entt::hashed_string_comparator> inputs;


  void serialize( const std::string& json ) const;

  void deserialize( entt::registry&,
                    entt::entity,
                    const std::string& json ) const;

  static void Register();
};

} // namespace Components

} // namespace ECS
