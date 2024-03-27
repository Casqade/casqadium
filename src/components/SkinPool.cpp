#include <cqde/components/SkinPool.hpp>

#include <cqde/json_helpers.hpp>

#include <entt/entity/registry.hpp>

#include <json/value.h>


namespace cqde::compos
{

const static Json::Value skinPoolJsonReference =
[]
{
  using ValueType = Json::ValueType;
  using namespace std::string_literals;

  Json::Value root = ValueType::objectValue;
  root.setComment("// root must be a JSON object"s,
                  Json::CommentPlacement::commentBefore);

  Json::Value& skins = root["skins"];
  skins = ValueType::objectValue;
  skins.setComment("// 'skins' must be a JSON object"s,
                    Json::CommentPlacement::commentBefore);

  Json::Value& skin = skins["cqde_json_anykey"];
  skin = ValueType::arrayValue;
  skin.setComment("// 'skins' member must be a JSON array"s,
                    Json::CommentPlacement::commentBefore);

  skin.append(ValueType::stringValue);
  skin.begin()->setComment("// 'skins' member element must be a JSON string"s,
                            Json::CommentPlacement::commentBefore);

  return root;
}();

Json::Value
SkinPool::serialize() const
{
  Json::Value json {};

  auto& jsonSkins = json["skins"];
  jsonSkins = Json::objectValue;

  for ( auto&& [meshId, textures] : skins )
  {
    auto& texturesJson = jsonSkins[meshId.str()];
    texturesJson = Json::arrayValue;

    for ( const auto& texture : textures )
      texturesJson.append(texture.str());
  }

  return json;
}

void
SkinPool::deserialize(
  entt::registry& registry,
  entt::entity entity,
  const Json::Value& json,
  const IdMap& idMap )
{
  jsonValidateObject(json, skinPoolJsonReference);

  auto& comp = registry.emplace_or_replace <SkinPool> (entity);

  comp.skins.clear();

  auto& jsonSkins = json["skins"];

  for ( const auto& meshId : jsonSkins.getMemberNames() )
    for ( auto& texture : jsonSkins[meshId] )
      comp.skins[meshId].push_back(texture.asString());
}

} // namespace cqde::compos
