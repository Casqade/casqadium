#include <ECS/Components/SceneNode.hpp>
#include <ECS/Components/Transform.hpp>

#include <entt/entt.hpp>


namespace ECS
{

namespace Components
{

glm::mat4
resolveWorldMatrix( entt::registry& registry, entt::entity entity )
{
  if ( registry.valid(entity) == false )
    return glm::mat4(1.0f);

  Transform& transform = registry.get <Transform> (entity);
  SceneNode& sceneNode = registry.get <SceneNode> (entity);

  return
      resolveWorldMatrix( registry, entity ) *
      glm::translate(glm::mat4(1.0f), transform.translation) *
      glm::mat4(transform.orientation) *
      glm::scale(glm::mat4(1.0f), sceneNode.scaleWorld);

}

SceneNode::SceneNode()
  : parent()
  , scaleWorld()
  , dirty(true)
  , modelWorldCache()
{}

glm::mat4
SceneNode::modelWorld( entt::registry& registry )
{
  if ( dirty == false)
    return modelWorldCache;

  entt::entity parentNode = parent.get();

  if ( registry.valid(parentNode) == false )
  {

  }

  Transform& parentTransform = registry.get <Transform> (parentNode);
  SceneNode& parentNode = registry.get <SceneNode> (parentNode);

  return modelWorldCache =
    parentNode.modelWorld(registry) *
    glm::translate(glm::mat4(1.0f), parentTransform.translation) *
    glm::mat4(parentTransform.orientation) *
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
