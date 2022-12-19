#pragma once

#include <cqde/alias.hpp>

#include <entt/fwd.hpp>

#include <json/forwards.h>


namespace cqde::compos
{

struct InteractionSource
{
  float radius {-1.0f};

  enum class Type
  {
    ClosestVisible,
    MousePos,
    ViewportCenter,

  } type {Type::ClosestVisible};


  InteractionSource() = default;


  static std::string InteractionTypeToString( const Type );
  static Type InteractionTypeFromString( const std::string& );


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
