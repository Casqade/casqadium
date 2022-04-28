#pragma once

#include <cqde/alias.hpp>

#include <entt/fwd.hpp>

#include <string>


namespace cqde::compos
{

struct Tag
{
  EntityId id;


  static Tag Generate( entt::registry&, entt::hashed_string = {} );


  void serialize( const std::string& json ) const;

  void deserialize( entt::registry&,
                    entt::entity,
                    const std::string& json ) const;

  static void Register();
};

} // namespace cqde::compos
