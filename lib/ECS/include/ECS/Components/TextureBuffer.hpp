#ifndef ECS_COMPONENTS_TEXTURE_BUFFER_HPP
#define ECS_COMPONENTS_TEXTURE_BUFFER_HPP

#include <entt/fwd.hpp>
#include <entt/core/hashed_string.hpp>

#include <vector>
#include <string>


namespace ECS
{

namespace Components
{

struct TextureBuffer
{
  std::vector <entt::hashed_string> textures;


  TextureBuffer();

  void serialize( const std::string& json ) const;

  void deserialize( entt::registry&,
                    entt::entity,
                    const std::string& json ) const;

  static void Register();
};

} // namespace Components

} // namespace ECS

#endif
