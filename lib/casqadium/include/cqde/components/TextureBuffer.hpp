#pragma once

#include <cqde/alias.hpp>

#include <entt/fwd.hpp>

#include <vector>
#include <string>


namespace cqde::compos
{

struct TextureBuffer
{
  std::vector <cqde::TextureId> textures;


  TextureBuffer();


  void serialize( const std::string& json ) const;

  void deserialize( entt::registry&,
                    entt::entity,
                    const std::string& json ) const;

  static void Register();
};

} // namespace cqde::compos
