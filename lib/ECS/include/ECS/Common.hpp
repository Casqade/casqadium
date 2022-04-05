#ifndef ECS_COMMON_HPP
#define ECS_COMMON_HPP

#include <entt/fwd.hpp>


namespace ECS
{

extern entt::entity AssetStorage;

void registryInit( entt::registry& registry );

} // namespace ECS

#endif
