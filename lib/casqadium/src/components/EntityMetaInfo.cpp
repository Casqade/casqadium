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
  root.setComment("// JSON root must be an object"s,
                   Json::CommentPlacement::commentBefore);

  root["packageId"] = ValueType::stringValue;
  root["packageId"].setComment("// 'packageId' must be a JSON string"s,
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
  const Json::Value& json )
{
  jsonValidateObject(json, entityMetaInfoJsonReference);

  auto& comp = registry.emplace <EntityMetaInfo> (entity);

  comp.packageId = json["packageId"].asString();
}

} // namespace cqde::compos
