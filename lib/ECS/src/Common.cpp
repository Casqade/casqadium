#include <ECS/Common.hpp>
#include <ECS/Components/InputController.hpp>
#include <ECS/Components/SceneNode.hpp>
#include <ECS/Components/Tag.hpp>
#include <ECS/Components/Transform.hpp>


namespace ECS
{

void
registryInit( entt::registry& registry )
{
  Components::InputController::Register();
  Components::SceneNode::Register();
  Components::Tag::Register();
  Components::Transform::Register();
}

} // namespace ECS


namespace entt
{

bool
hashed_string_comparator::operator () (
  const entt::hashed_string& lhs,
  const entt::hashed_string& rhs ) const
{
  return lhs.value() < rhs.value();
}

} // namespace entt
