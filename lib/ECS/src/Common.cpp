#include <ECS/Common.hpp>
#include <ECS/Components/SceneNode.hpp>
#include <ECS/Components/Tag.hpp>
#include <ECS/Components/Transform.hpp>


namespace ECS
{

entt::entity AssetStorage{};

void
registryInit( entt::registry& registry )
{
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
