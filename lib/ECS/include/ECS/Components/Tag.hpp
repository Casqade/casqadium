#pragma once

#include <ECS/Alias.hpp>

#include <entt/fwd.hpp>

#include <string>


namespace ECS
{

namespace Components
{

struct Tag
{
  EntityId id;


  static Tag Generate( entt::hashed_string = {} );


  void serialize( const std::string& json ) const;

  void deserialize( entt::registry&,
                    entt::entity,
                    const std::string& json ) const;

  static void Register();
};

} // namespace Components

} // namespace ECS
