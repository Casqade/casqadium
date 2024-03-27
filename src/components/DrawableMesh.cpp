#include <cqde/components/DrawableMesh.hpp>

#include <cqde/json_helpers.hpp>

#include <entt/entity/registry.hpp>

#include <json/value.h>


namespace cqde::compos
{

const static Json::Value drawableMeshJsonReference =
[]
{
  using ValueType = Json::ValueType;
  using namespace std::string_literals;

  Json::Value root = ValueType::objectValue;
  root.setComment("// root must be a JSON object"s,
                   Json::CommentPlacement::commentBefore);

  auto& meshId = root["meshId"];
  meshId = ValueType::stringValue;
  meshId.setComment("// 'meshId' must be a JSON string"s,
                    Json::CommentPlacement::commentBefore);

  Json::Value& textureId = root["textureId"];
  textureId = ValueType::stringValue;
  textureId.setComment("// 'textureId' must be a JSON string"s,
                      Json::CommentPlacement::commentBefore);

  return root;
}();

Json::Value
DrawableMesh::serialize() const
{
  Json::Value json {};

  json["meshId"] = meshId.str();
  json["textureId"] = textureId.str();

  return json;
}

void
DrawableMesh::deserialize(
  entt::registry& registry,
  entt::entity entity,
  const Json::Value& json,
  const IdMap& idMap )
{
  jsonValidateObject(json, drawableMeshJsonReference);

  auto& comp = registry.emplace_or_replace <DrawableMesh> (entity);

  comp.meshId = json["meshId"].asString();
  comp.textureId = json["textureId"].asString();
}

} // namespace cqde::compos
