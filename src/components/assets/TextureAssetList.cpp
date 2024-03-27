#include <cqde/components/assets/TextureAssetList.hpp>

#include <cqde/json_helpers.hpp>

#include <entt/entity/registry.hpp>

#include <json/value.h>


namespace cqde::compos
{

const static Json::Value textureAssetListJsonReference =
[]
{
  using ValueType = Json::ValueType;
  using namespace std::string_literals;

  Json::Value root = ValueType::objectValue;
  root.setComment("// root must be a JSON object"s,
                  Json::CommentPlacement::commentBefore);

  Json::Value& textures = root["textures"];
  textures = ValueType::arrayValue;
  textures.setComment("// 'textures' must be a JSON array"s,
                      Json::CommentPlacement::commentBefore);

  textures.append(ValueType::stringValue);
  textures.begin()->setComment("// 'textures' element must be a JSON string"s,
                                Json::CommentPlacement::commentBefore);

  return root;
}();

Json::Value
TextureAssetList::serialize() const
{
  Json::Value json {};

  auto& jsonTextures = json["textures"];
  jsonTextures = Json::arrayValue;

  for ( const auto& texture : textures )
    jsonTextures.append(texture.str());

  return json;
}

void
TextureAssetList::deserialize(
  entt::registry& registry,
  entt::entity entity,
  const Json::Value& json,
  const IdMap& idMap )
{
  jsonValidateObject(json, textureAssetListJsonReference);

  auto& comp = registry.emplace_or_replace <TextureAssetList> (entity);

  for ( const auto& texture : json["textures"] )
    comp.textures.insert(texture.asString());
}

} // namespace cqde::compos
