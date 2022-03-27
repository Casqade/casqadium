#include <ECS/Common.hpp>
#include <ECS/Components/SceneNode.hpp>
#include <ECS/Components/Tag.hpp>
#include <ECS/Components/Transform.hpp>


namespace ECS
{

void
registryInit( entt::registry& registry )
{
  Components::SceneNode::Register();
  Components::Tag::Register();
  Components::Transform::Register();
}

} // namespace ECS
