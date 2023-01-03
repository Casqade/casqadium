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
  root.setComment("// root must be a JSON object"s,
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
  const Json::Value& json,
  const std::unordered_map <EntityId, EntityId, identifier_hash>& idMap )
{
  jsonValidateObject(json, SceneNodeJsonReference);

  auto& comp = registry.emplace_or_replace <SceneNode> (entity);

  comp.parent = {json["parent"].asString()};

  if ( idMap.count(comp.parent.id) > 0 )
    comp.parent = idMap.at(comp.parent.id);

  else
  {
    const auto eParent = comp.parent.get(registry);

    if ( eParent != entt::null )
    {
      auto& cNode = registry.get <SceneNode> (eParent);
      cNode.children.insert(registry.get <Tag> (entity).id);
    }
  }

  for ( const auto& child : json["children"] )
  {
    auto childId = child.asString();

    if ( idMap.count(childId) > 0 )
      childId = idMap.at(childId).str();

    comp.children.insert({childId});
  }
}

} // namespace cqde::compos

namespace cqde
{

size_t
ChildNodeDepth(
  const entt::registry& registry,
  const compos::SceneNode* node )
{
  using compos::SceneNode;

  if ( node == nullptr )
    return 0;

  const auto eParent = node->parent.get(registry);

  if ( eParent == entt::null )
    return 1;

  const auto cParent = registry.try_get <SceneNode> (eParent);

  return ChildNodeDepth(registry, cParent) + 1;
}

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

  const auto ancestor = cNode.parent.get(registry);

  return CanAddChildNode(registry, ancestor, childId);
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

  DetachChildNode(registry, cChildNode.parent.get(registry), eChild);

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

  cParentNode.children.erase(cChildTag.id);
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

  const auto& cNode = registry.get <SceneNode> (eChild);

  const auto children = cNode.children;

  for ( const auto& child : children )
    DestroyChildNode(registry, eChild, child.get(registry));

  registry.ctx().at <EntityManager> ().removeLater(eChild);
}

void
RootifyChildNode(
  entt::registry& registry,
  const entt::entity entity )
{
  using compos::SceneNode;

  const auto& cNode = registry.get <SceneNode> (entity);

  DetachChildNode(registry,
                  cNode.parent.get(registry),
                  entity);

  const auto children = cNode.children;

  for ( const auto& child : children )
    DetachChildNode(registry, entity,
                    child.get(registry));
}

void SerializeChildNode(
  const entt::registry& registry,
  Json::Value& json,
  const entt::entity entity,
  const std::unordered_set <ComponentType>& excludedComponents )
{
  using compos::SceneNode;
  using types::EntityManager;

  const auto& entityManager = registry.ctx().at <EntityManager> ();

  entityManager.entitySerialize(registry, json, entity,
                                excludedComponents);

  const auto& cNode = registry.get <SceneNode> (entity);

  for ( const auto& child : cNode.children )
  {
    const auto childEntity = child.get(registry);

    if ( childEntity != entt::null )
      SerializeChildNode(registry, json, childEntity,
                         excludedComponents);
  }
}

} // namespace cqde
