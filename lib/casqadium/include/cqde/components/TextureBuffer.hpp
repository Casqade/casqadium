#pragma once

#include <cqde/alias.hpp>

#include <entt/fwd.hpp>

#include <json/forwards.h>

#include <vector>


namespace cqde::compos
{

struct TextureBuffer
{
  std::vector <cqde::TextureId> textures {};


  TextureBuffer() = default;


  void serialize( Json::Value& ) const;

  void deserialize( entt::registry&,
                    entt::entity,
                    const Json::Value& ) const;

  static void Register();
};

} // namespace cqde::compos
