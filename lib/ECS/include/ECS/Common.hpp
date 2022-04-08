#ifndef ECS_COMMON_HPP
#define ECS_COMMON_HPP

#include <entt/fwd.hpp>
#include <entt/core/hashed_string.hpp>


namespace ECS
{

extern entt::entity AssetStorage;

void registryInit( entt::registry& registry );

} // namespace ECS

namespace entt
{

class hashed_string_comparator
{
public:
  bool operator () ( const entt::hashed_string& lhs, const entt::hashed_string& rhs ) const;
};

}

#endif
