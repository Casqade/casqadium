#include <cqde/components/TextureTint.hpp>

#include <cqde/json_helpers.hpp>

#include <entt/entity/registry.hpp>

#include <json/value.h>


namespace cqde::compos
{

const static Json::Value textureTintJsonReference =
[]
{
  using ValueType = Json::ValueType;
  using namespace std::string_literals;

  Json::Value root = ValueType::objectValue;
  root.setComment("// root must be a JSON object"s,
                   Json::CommentPlacement::commentBefore);

  auto& tint = root["tint"];
  tint = ValueType::uintValue;
  tint.setComment("// 'tint' must be a JSON unsigned integer"s,
                  Json::CommentPlacement::commentBefore);

  return root;
}();

Json::Value
TextureTint::serialize() const
{
  Json::Value json {};
  json["tint"] = tint.n;

  return json;
}

void
TextureTint::deserialize(
  entt::registry& registry,
  entt::entity entity,
  const Json::Value& json,
  const IdMap& idMap )
{
  jsonValidateObject(json, textureTintJsonReference);

  auto& comp = registry.emplace_or_replace <TextureTint> (entity);

  comp.tint = json["tint"].asUInt();
}

} // namespace cqde::compos
