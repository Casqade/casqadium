#include <cqde/components/WantsCursorOverridden.hpp>

#include <cqde/json_helpers.hpp>

#include <entt/entity/registry.hpp>

#include <json/value.h>


namespace cqde::compos
{

const static Json::Value wantsCursorOverriddenJsonReference =
[]
{
  using ValueType = Json::ValueType;
  using namespace std::string_literals;

  Json::Value root = ValueType::objectValue;
  root.setComment("// root must be a JSON object"s,
                   Json::CommentPlacement::commentBefore);

  auto& cursor = root["cursor"];
  cursor = ValueType::stringValue;
  cursor.setComment("// 'cursor' must be a JSON string"s,
                    Json::CommentPlacement::commentBefore);

  return root;
}();

Json::Value
WantsCursorOverridden::serialize() const
{
  Json::Value json {};
  json["cursor"] = cursor.str();

  return json;
}

void
WantsCursorOverridden::deserialize(
  entt::registry& registry,
  entt::entity entity,
  const Json::Value& json,
  const std::unordered_map <EntityId, EntityId,
                            identifier_hash>& idMap )
{
  jsonValidateObject(json, wantsCursorOverriddenJsonReference);

  auto& comp = registry.emplace_or_replace <WantsCursorOverridden> (entity);

  comp.cursor = json["cursor"].asString();
}

} // namespace cqde::compos
