#include <ECS/Components/SceneNode.hpp>

#include <glm/gtx/quaternion.hpp>

#include <entt/entt.hpp>


namespace ECS
{

namespace Components
{

SceneNode::SceneNode()
  : parent()
  , children()
{}

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
  factory.data <&SceneNode::parent> ("parent"_hs);
  factory.func <&SceneNode::serialize> ("serialze"_hs);
  factory.func <&SceneNode::deserialize> ("deserialize"_hs);
}

} // namespace Components

} // namespace ECS
