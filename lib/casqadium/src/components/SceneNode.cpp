#include <cqde/components/SceneNode.hpp>
#include <cqde/components/Tag.hpp>

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

  root["parent"] = ValueType::stringValue;
  root["parent"].setComment("// 'parent' must be a JSON string"s,
                            Json::CommentPlacement::commentBefore);

  root["children"] = ValueType::arrayValue;
  root["children"].setComment("// 'children' must be a JSON array"s,
                              Json::CommentPlacement::commentBefore);

  root["children"].append(ValueType::stringValue);
  root["children"].begin()->setComment("// 'children' element must be a JSON string"s,
                                        Json::CommentPlacement::commentBefore);

  return root;
}();

Json::Value
SceneNode::serialize() const
{
  Json::Value json {};

  json["parent"] = parent.id.str();

  for ( const auto& child : children )
    json["children"].append(child.id.str());

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
