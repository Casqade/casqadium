#include <ECS/Common.hpp>
#include <ECS/Components/Tag.hpp>


namespace ECS
{

void
registryInit( entt::registry& registry )
{
  Components::Tag::Register();
}

} // namespace ECS
