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


  void ui_edit_props( const entt::entity,
                      const entt::registry& );

  Json::Value serialize() const;

  static void deserialize(
    entt::registry&,
    entt::entity,
    const Json::Value& );
};

} // namespace cqde::compos
