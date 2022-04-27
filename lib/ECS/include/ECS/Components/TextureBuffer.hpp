#pragma once

#include <ECS/Alias.hpp>

#include <entt/fwd.hpp>

#include <vector>
#include <string>


namespace ECS
{

namespace Components
{

struct TextureBuffer
{
  std::vector <ECS::TextureId> textures;


  TextureBuffer();


  void serialize( const std::string& json ) const;

  void deserialize( entt::registry&,
                    entt::entity,
                    const std::string& json ) const;

  static void Register();
};

} // namespace Components

} // namespace ECS
