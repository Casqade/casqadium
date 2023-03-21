#include <cqde/components/EntityMetaInfo.hpp>

#include <cqde/json_helpers.hpp>

#include <entt/entity/registry.hpp>

#include <json/value.h>


namespace cqde::compos
{

const static Json::Value entityMetaInfoJsonReference =
[]
{
  using ValueType = Json::ValueType;
  using namespace std::string_literals;

  Json::Value root = ValueType::objectValue;
  root.setComment("// root must be a JSON object"s,
                  Json::CommentPlacement::commentBefore);

  auto& packageId = root["packageId"];
  packageId = ValueType::stringValue;
  packageId.setComment("// 'packageId' must be a JSON string"s,
                        Json::CommentPlacement::commentBefore);

  return root;
}();

Json::Value
EntityMetaInfo::serialize() const
{
  Json::Value json {};
  json["packageId"] = packageId.str();

  return json;
}

void
EntityMetaInfo::deserialize(
  entt::registry& registry,
  entt::entity entity,
  const Json::Value& json,
  const std::unordered_map <EntityId, EntityId,
                            identifier_hash>& idMap )
{
  jsonValidateObject(json, entityMetaInfoJsonReference);

  auto& comp = registry.emplace_or_replace <EntityMetaInfo> (entity);

  comp.packageId = json["packageId"].asString();
}

} // namespace cqde::compos
