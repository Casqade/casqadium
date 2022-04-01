#include <ECS/Components/SceneNode.hpp>
#include <ECS/Components/Transform.hpp>

#include <glm/gtx/quaternion.hpp>

#include <entt/entt.hpp>


namespace ECS
{

glm::mat4
ResolveWorldMatrix(
  entt::registry& registry,
  const Components::Transform& transform,
  const Components::SceneNode& sceneNode )
{
  return sceneNode.modelWorld(registry) * transform.modelLocal();
}

namespace Components
{

glm::mat4
SceneNode::modelWorld( entt::registry& registry ) const
{
  entt::entity parentEntity = parent.get();

  if ( registry.valid(parentEntity) == false )
    return glm::scale(glm::mat4(1.0f), scaleWorld);

  Transform& parentTransform = registry.get <Transform> (parentEntity);
  SceneNode& parentNode = registry.get <SceneNode> (parentEntity);

  return
    parentNode.modelWorld(registry) *
    glm::translate(glm::mat4(1.0f), parentTransform.translation) *
    glm::toMat4(parentTransform.orientation) *
    glm::scale(glm::mat4(1.0f), scaleWorld);
}

void
SceneNode::serialize( const std::string& json ) const
{}

void
SceneNode::deserialize(
  entt::registry& registry,
  entt::entity entity,
  const std::string& content ) const
{
  auto& comp = registry.emplace <SceneNode> (entity);
}

void
SceneNode::Register()
{
  using namespace entt::literals;

  auto factory = entt::meta <SceneNode> ();
  factory.type(entt::hashed_string{"SceneNode"});
  factory.data <&SceneNode::scaleWorld> ("scaleWorld"_hs);
  factory.func <&SceneNode::serialize> ("serialze"_hs);
  factory.func <&SceneNode::deserialize> ("deserialize"_hs);
}

} // namespace Components

} // namespace ECS
