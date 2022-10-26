#include <cqde/components/assets/TerrainAssetUnloadList.hpp>

#include <cqde/json_helpers.hpp>

#include <entt/entity/registry.hpp>

#include <json/value.h>


namespace cqde::compos
{

const static Json::Value terrainAssetUnloadListJsonReference =
[]
{
  using ValueType = Json::ValueType;
  using namespace std::string_literals;

  Json::Value root = ValueType::objectValue;
  root.setComment("// root must be a JSON object"s,
                  Json::CommentPlacement::commentBefore);

  Json::Value& terrain = root["terrainToUnload"];
  terrain = ValueType::arrayValue;
  terrain.setComment("// 'terrainToUnload' must be a JSON array"s,
                      Json::CommentPlacement::commentBefore);

  terrain.append(ValueType::stringValue);
  terrain.begin()->setComment("// 'terrainToUnload' element must be a JSON string"s,
                                Json::CommentPlacement::commentBefore);

  return root;
}();

Json::Value
TerrainAssetUnloadList::serialize() const
{
  Json::Value json {};

  auto& jsonTerrain = json["terrainToUnload"];
  jsonTerrain = Json::arrayValue;

  for ( const auto& terrain : terrainToUnload )
    jsonTerrain.append(terrain.str());

  return json;
}

void
TerrainAssetUnloadList::deserialize(
  entt::registry& registry,
  entt::entity entity,
  const Json::Value& json,
  const std::unordered_map <EntityId, EntityId,
                            identifier_hash>& idMap )
{
  jsonValidateObject(json, terrainAssetUnloadListJsonReference);

  auto& comp = registry.emplace_or_replace <TerrainAssetUnloadList> (entity);

  for ( const auto& terrain : json["terrainToUnload"] )
    comp.terrainToUnload.insert(terrain.asString());
}

} // namespace cqde::compos
