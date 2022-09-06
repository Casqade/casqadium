#include <cqde/components/SceneNode.hpp>
#include <cqde/components/Tag.hpp>

#include <cqde/types/EntityManager.hpp>

#include <cqde/common.hpp>
#include <cqde/json_helpers.hpp>

#include <entt/entity/registry.hpp>

#include <json/value.h>


namespace cqde::compos
{

const static Json::Value SceneNodeJsonReference =
[]
{
  using ValueType = Json::ValueType;
  using namespace std::string_literals;

  Json::Value root = ValueType::objectValue;
  root.setComment("// JSON root must be an object"s,
                  Json::CommentPlacement::commentBefore);

  auto& parent = root["parent"];
  parent = ValueType::stringValue;
  parent.setComment("// 'parent' must be a JSON string"s,
                    Json::CommentPlacement::commentBefore);

  auto& children = root["children"];
  children = ValueType::arrayValue;
  children.setComment("// 'children' must be a JSON array"s,
                      Json::CommentPlacement::commentBefore);

  children.append(ValueType::stringValue);
  children.begin()->setComment("// 'children' element must be a JSON string"s,
                                Json::CommentPlacement::commentBefore);

  return root;
}();

Json::Value
SceneNode::serialize() const
{
  Json::Value json {};

  json["parent"] = parent.id.str();

  auto& jsonChildren = json["children"];
  jsonChildren = Json::arrayValue;

  for ( const auto& child : children )
    jsonChildren.append(child.id.str());

  return json;
}

void
SceneNode::deserialize(
  entt::registry& registry,
  entt::entity entity,
  const Json::Value& json )
{
  jsonValidateObject(json, SceneNodeJsonReference);

  auto& comp = registry.emplace <SceneNode> (entity);

  comp.parent = Tag{json["parent"].asString()};

  for ( const auto& child : json["children"] )
    comp.children.insert({Tag{child.asString()}});
}

} // namespace cqde::compos

namespace cqde
{

bool
CanAddChildNode(
  const entt::registry& registry,
  const entt::entity eParent,
  const EntityId& childId )
{
  using compos::Tag;
  using compos::SceneNode;

  if ( eParent == entt::null )
    return true;

  const auto& cTag = registry.get <Tag> (eParent);

  if ( cTag.id == childId )
    return false;

  const auto& cNode = registry.get <SceneNode> (eParent);

  const auto superParent = cNode.parent.get_if_valid(registry);

  return CanAddChildNode(registry, superParent, childId);
}

void
AttachChildNode(
  entt::registry& registry,
  const entt::entity eParent,
  const entt::entity eChild )
{
  using compos::Tag;
  using compos::SceneNode;

  CQDE_ASSERT_DEBUG(eChild != entt::null, return);

  SceneNode& cChildNode = registry.get <SceneNode> (eChild);

  DetachChildNode(registry, cChildNode.parent.get_if_valid(registry), eChild);

  if ( eParent == entt::null )
  {
    cChildNode.parent = {};
    return;
  }

  SceneNode& cParentNode = registry.get <SceneNode> (eParent);

  const auto& cParentTag = registry.get <Tag> (eParent);
  const auto& cChildTag = registry.get <Tag> (eChild);

  cParentNode.children.insert(cChildTag);

  cChildNode.parent = cParentTag;
}

void
DetachChildNode(
  entt::registry& registry,
  const entt::entity eParent,
  const entt::entity eChild )
{
  using compos::Tag;
  using compos::SceneNode;

  if ( eParent == entt::null ||
       eChild == entt::null )
    return;

  SceneNode& cParentNode = registry.get <SceneNode> (eParent);
  SceneNode& cChildNode = registry.get <SceneNode> (eChild);

  const Tag& cChildTag = registry.get <Tag> (eChild);

  cParentNode.children.erase(cChildTag);
  cChildNode.parent = {};
}

void
DestroyChildNode(
  entt::registry& registry,
  const entt::entity eParent,
  const entt::entity eChild )
{
  using compos::Tag;
  using compos::SceneNode;
  using types::EntityManager;

  if ( eChild == entt::null )
    return;

  DetachChildNode(registry, eParent, eChild);

  const auto& cTag  = registry.get <Tag> (eChild);
  const auto& cNode = registry.get <SceneNode> (eChild);

  const auto children = cNode.children;

  for ( const auto& child : children )
    DestroyChildNode(registry, eChild, child.get_if_valid(registry));

  registry.ctx().at <EntityManager> ().removeLater(eChild);
}

} // namespace cqde
