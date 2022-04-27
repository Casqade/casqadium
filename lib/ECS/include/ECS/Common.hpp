#pragma once

#include <entt/fwd.hpp>
#include <entt/core/hashed_string.hpp>


namespace ECS
{

void registryInit( entt::registry& registry );

} // namespace ECS

namespace entt
{

class hashed_string_comparator
{
public:
  bool operator () ( const entt::hashed_string& lhs, const entt::hashed_string& rhs ) const;
};

} // namespace entt

