#pragma once

#include <cqde/alias.hpp>

#include <entt/fwd.hpp>
#include <entt/entity/entity.hpp>

#include <json/forwards.h>

#include <unordered_set>


namespace cqde::compos
{

struct InteractionSource
{
  std::unordered_set <ActionId, identifier_hash> actions {};

  entt::entity listener {entt::null};

  enum class Type
  {
    PhysicsProbe,
    ViewportCenter,
    MousePos,

  } type {Type::PhysicsProbe};

  float radius {-1.0f};


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
    const IdMap& = {} );
};

} // namespace cqde::compos
