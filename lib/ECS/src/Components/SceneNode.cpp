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

void
AttachChildNode(
  entt::registry& registry,
  const entt::entity eParent,
  const entt::entity eChild )
{
  SceneNode& cParentNode = registry.get <SceneNode> (eParent);
  SceneNode& cChildNode = registry.get <SceneNode> (eChild);

  Tag& cParentTag = registry.get <Tag> (eParent);
  Tag& cChildTag = registry.get <Tag> (eChild);

}

entt::entity
DetachChildNode(
  entt::registry& registry,
  const entt::entity eParent,
  const entt::entity eChild )
{
  SceneNode& cParentNode = registry.get <SceneNode> (eParent);
  SceneNode& cChildNode = registry.get <SceneNode> (eChild);

  Tag& cParentTag = registry.get <Tag> (eParent);
  Tag& cChildTag = registry.get <Tag> (eChild);


  return eChild;
}

void
RemoveChildNode(
  entt::registry& registry,
  const entt::entity eParent,
  const entt::entity eChild )
{
  SceneNode& cParentNode = registry.get <SceneNode> (eParent);

  Tag& cChildTag = registry.get <Tag> (eChild);

  cParentNode.children.erase(EntityReference(cChildTag));
  registry.destroy(eChild);
}

} // namespace Components

} // namespace ECS
