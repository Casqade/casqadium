#include <cqde/components/assets/GeometryAssetUnloadList.hpp>

#include <cqde/json_helpers.hpp>

#include <entt/entity/registry.hpp>

#include <json/value.h>


namespace cqde::compos
{

const static Json::Value geometryAssetUnloadListJsonReference =
[]
{
  using ValueType = Json::ValueType;
  using namespace std::string_literals;

  Json::Value root = ValueType::objectValue;
  root.setComment("// root must be a JSON object"s,
                  Json::CommentPlacement::commentBefore);

  Json::Value& geometry = root["geometryToUnload"];
  geometry = ValueType::arrayValue;
  geometry.setComment("// 'geometryToUnload' must be a JSON array"s,
                      Json::CommentPlacement::commentBefore);

  geometry.append(ValueType::stringValue);
  geometry.begin()->setComment("// 'geometryToUnload' element must be a JSON string"s,
                                Json::CommentPlacement::commentBefore);

  return root;
}();

Json::Value
GeometryAssetUnloadList::serialize() const
{
  Json::Value json {};

  auto& jsonGeometry = json["geometryToUnload"];
  jsonGeometry = Json::arrayValue;

  for ( const auto& geometry : geometryToUnload )
    jsonGeometry.append(geometry.str());

  return json;
}

void
GeometryAssetUnloadList::deserialize(
  entt::registry& registry,
  entt::entity entity,
  const Json::Value& json,
  const std::unordered_map <EntityId, EntityId,
                            identifier_hash>& idMap )
{
  jsonValidateObject(json, geometryAssetUnloadListJsonReference);

  auto& comp = registry.emplace_or_replace <GeometryAssetUnloadList> (entity);

  for ( const auto& geometry : json["geometryToUnload"] )
    comp.geometryToUnload.insert(geometry.asString());
}

} // namespace cqde::compos
