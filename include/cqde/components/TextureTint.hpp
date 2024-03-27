#pragma once

#include <cqde/alias.hpp>

#include <olcPGE/Pixel.hpp>

#include <entt/fwd.hpp>

#include <json/forwards.h>


namespace cqde::compos
{

struct TextureTint
{
  olc::Pixel tint {olc::WHITE};


  TextureTint() = default;


  void ui_edit_props( const entt::entity,
                      const entt::registry& );

  Json::Value serialize() const;

  static void deserialize(
    entt::registry&,
    entt::entity,
    const Json::Value&,
    const IdMap& = {} );
};

} // namespace cqde::compos
