#include <cqde/components/assets/GeometryAssetList.hpp>

#include <cqde/json_helpers.hpp>

#include <entt/entity/registry.hpp>

#include <json/value.h>


namespace cqde::compos
{

const static Json::Value geometryAssetListJsonReference =
[]
{
  using ValueType = Json::ValueType;
  using namespace std::string_literals;

  Json::Value root = ValueType::objectValue;
  root.setComment("// root must be a JSON object"s,
                  Json::CommentPlacement::commentBefore);

  Json::Value& geometry = root["geometry"];
  geometry = ValueType::arrayValue;
  geometry.setComment("// 'geometry' must be a JSON array"s,
                      Json::CommentPlacement::commentBefore);

  geometry.append(ValueType::stringValue);
  geometry.begin()->setComment("// 'geometry' element must be a JSON string"s,
                                Json::CommentPlacement::commentBefore);

  return root;
}();

Json::Value
GeometryAssetList::serialize() const
{
  Json::Value json {};

  auto& jsonGeometry = json["geometry"];
  jsonGeometry = Json::arrayValue;

  for ( const auto& geometry : geometry )
    jsonGeometry.append(geometry.str());

  return json;
}

void
GeometryAssetList::deserialize(
  entt::registry& registry,
  entt::entity entity,
  const Json::Value& json,
  const std::unordered_map <EntityId, EntityId,
                            identifier_hash>& idMap )
{
  jsonValidateObject(json, geometryAssetListJsonReference);

  auto& comp = registry.emplace_or_replace <GeometryAssetList> (entity);

  for ( const auto& geometry : json["geometry"] )
    comp.geometry.insert(geometry.asString());
}

} // namespace cqde::compos
