#include <cqde/components/SceneNode.hpp>
#include <cqde/components/Tag.hpp>

#include <entt/entt.hpp>

#include <json/value.h>


namespace cqde::compos
{

SceneNode::SceneNode()
  : parent()
  , children()
{}

void
SceneNode::serialize( Json::Value& json ) const
{}

void
SceneNode::deserialize(
  entt::registry& registry,
  entt::entity entity,
  const Json::Value& content ) const
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
  factory.func <&SceneNode::serialize> ("serialize"_hs);
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

  cParentNode.children.insert(types::EntityReference(cChildTag));
  cChildNode.parent = types::EntityReference(cParentTag);
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

  cParentNode.children.erase(types::EntityReference(cChildTag));
  cChildNode.parent = types::EntityReference();

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

  cParentNode.children.erase(types::EntityReference(cChildTag));

  cChildTag.invalidate(registry);
  registry.destroy(eChild);
}

} // namespace cqde::compos
