#include <cqde/components/GeometryBuffer.hpp>

#include <cqde/json_helpers.hpp>

#include <entt/entity/registry.hpp>

#include <json/value.h>


namespace cqde::compos
{

const static Json::Value geometryBufferJsonReference =
[]
{
  using ValueType = Json::ValueType;
  using namespace std::string_literals;

  Json::Value root = ValueType::objectValue;
  root.setComment("// root must be a JSON object"s,
                   Json::CommentPlacement::commentBefore);

  auto& buffer = root["buffer"];
  buffer = ValueType::stringValue;
  buffer.setComment("// 'buffer' must be a JSON string"s,
                    Json::CommentPlacement::commentBefore);

  return root;
}();

Json::Value
GeometryBuffer::serialize() const
{
  Json::Value json {};
  json["buffer"] = buffer.str();

  return json;
}

void
GeometryBuffer::deserialize(
  entt::registry& registry,
  entt::entity entity,
  const Json::Value& json,
  const std::unordered_map <EntityId, EntityId,
                            identifier_hash>& idMap )
{
  jsonValidateObject(json, geometryBufferJsonReference);

  auto& comp = registry.emplace_or_replace <GeometryBuffer> (entity);

  comp.buffer = json["buffer"].asString();
}

} // namespace cqde::compos
