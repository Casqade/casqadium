#include <cqde/components/assets/TerrainAssetLoadList.hpp>

#include <cqde/json_helpers.hpp>

#include <entt/entity/registry.hpp>

#include <json/value.h>


namespace cqde::compos
{

const static Json::Value terrainAssetLoadListJsonReference =
[]
{
  using ValueType = Json::ValueType;
  using namespace std::string_literals;

  Json::Value root = ValueType::objectValue;
  root.setComment("// root must be a JSON object"s,
                  Json::CommentPlacement::commentBefore);

  Json::Value& terrain = root["terrainToLoad"];
  terrain = ValueType::arrayValue;
  terrain.setComment("// 'terrainToLoad' must be a JSON array"s,
                      Json::CommentPlacement::commentBefore);

  terrain.append(ValueType::stringValue);
  terrain.begin()->setComment("// 'terrainToLoad' element must be a JSON string"s,
                              Json::CommentPlacement::commentBefore);

  return root;
}();

Json::Value
TerrainAssetLoadList::serialize() const
{
  Json::Value json {};

  auto& jsonTerrain = json["terrainToLoad"];
  jsonTerrain = Json::arrayValue;

  for ( const auto& terrain : terrainToLoad )
    jsonTerrain.append(terrain.str());

  return json;
}

void
TerrainAssetLoadList::deserialize(
  entt::registry& registry,
  entt::entity entity,
  const Json::Value& json,
  const std::unordered_map <EntityId, EntityId,
                            identifier_hash>& idMap )
{
  jsonValidateObject(json, terrainAssetLoadListJsonReference);

  auto& comp = registry.emplace_or_replace <TerrainAssetLoadList> (entity);

  for ( const auto& terrain : json["terrainToLoad"] )
    comp.terrainToLoad.insert(terrain.asString());
}

} // namespace cqde::compos
