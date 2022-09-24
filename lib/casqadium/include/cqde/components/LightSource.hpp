#pragma once

#include <cqde/alias.hpp>

#include <olcPGE/olcPixelGameEngine.hpp>

#include <entt/fwd.hpp>

#include <json/forwards.h>


namespace cqde::compos
{

struct LightSource
{
  olc::Pixel color {olc::WHITE};

  float attenuationConstant {1.0f};
  float attenuationLinear {0.0f};
  float attenuationQuadratic {0.0f};

  float radius {100.0f};

  enum class Type
  {
    Ambient,
    Positional,
    Directional,

  } type {};


  LightSource() = default;


  static std::string LightSourceTypeToString( const Type );
  static Type LightSourceTypeFromString( const std::string& );


  void ui_edit_props( const entt::entity,
                      const entt::registry& );

  Json::Value serialize() const;

  static void deserialize(
    entt::registry&,
    entt::entity,
    const Json::Value&,
    const std::unordered_map <EntityId, EntityId,
                              identifier_hash>& idMap = {} );
};

} // namespace cqde::compos
