#include <cqde/components/assets/GeometryAssetLoadList.hpp>

#include <cqde/json_helpers.hpp>

#include <entt/entity/registry.hpp>

#include <json/value.h>


namespace cqde::compos
{

const static Json::Value geometryAssetLoadListJsonReference =
[]
{
  using ValueType = Json::ValueType;
  using namespace std::string_literals;

  Json::Value root = ValueType::objectValue;
  root.setComment("// root must be a JSON object"s,
                  Json::CommentPlacement::commentBefore);

  Json::Value& geometry = root["geometryToLoad"];
  geometry = ValueType::arrayValue;
  geometry.setComment("// 'geometryToLoad' must be a JSON array"s,
                      Json::CommentPlacement::commentBefore);

  geometry.append(ValueType::stringValue);
  geometry.begin()->setComment("// 'geometryToLoad' element must be a JSON string"s,
                                Json::CommentPlacement::commentBefore);

  return root;
}();

Json::Value
GeometryAssetLoadList::serialize() const
{
  Json::Value json {};

  auto& jsonGeometry = json["geometryToLoad"];
  jsonGeometry = Json::arrayValue;

  for ( const auto& geometry : geometryToLoad )
    jsonGeometry.append(geometry.str());

  return json;
}

void
GeometryAssetLoadList::deserialize(
  entt::registry& registry,
  entt::entity entity,
  const Json::Value& json,
  const std::unordered_map <EntityId, EntityId,
                            identifier_hash>& idMap )
{
  jsonValidateObject(json, geometryAssetLoadListJsonReference);

  auto& comp = registry.emplace_or_replace <GeometryAssetLoadList> (entity);

  for ( const auto& geometry : json["geometryToLoad"] )
    comp.geometryToLoad.insert(geometry.asString());
}

} // namespace cqde::compos
