#include <cqde/components/assets/TerrainAssetList.hpp>

#include <cqde/json_helpers.hpp>

#include <entt/entity/registry.hpp>

#include <json/value.h>


namespace cqde::compos
{

const static Json::Value terrainAssetListJsonReference =
[]
{
  using ValueType = Json::ValueType;
  using namespace std::string_literals;

  Json::Value root = ValueType::objectValue;
  root.setComment("// root must be a JSON object"s,
                  Json::CommentPlacement::commentBefore);

  Json::Value& terrain = root["terrain"];
  terrain = ValueType::arrayValue;
  terrain.setComment("// 'terrain' must be a JSON array"s,
                      Json::CommentPlacement::commentBefore);

  terrain.append(ValueType::stringValue);
  terrain.begin()->setComment("// 'terrain' element must be a JSON string"s,
                              Json::CommentPlacement::commentBefore);

  return root;
}();

Json::Value
TerrainAssetList::serialize() const
{
  Json::Value json {};

  auto& jsonTerrain = json["terrain"];
  jsonTerrain = Json::arrayValue;

  for ( const auto& terrain : terrain )
    jsonTerrain.append(terrain.str());

  return json;
}

void
TerrainAssetList::deserialize(
  entt::registry& registry,
  entt::entity entity,
  const Json::Value& json,
  const IdMap& idMap )
{
  jsonValidateObject(json, terrainAssetListJsonReference);

  auto& comp = registry.emplace_or_replace <TerrainAssetList> (entity);

  for ( const auto& terrain : json["terrain"] )
    comp.terrain.insert(terrain.asString());
}

} // namespace cqde::compos
