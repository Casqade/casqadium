#include <cqde/components/assets/TextureAssetLoadList.hpp>

#include <cqde/json_helpers.hpp>

#include <entt/entity/registry.hpp>

#include <json/value.h>


namespace cqde::compos
{

const static Json::Value textureAssetLoadListJsonReference =
[]
{
  using ValueType = Json::ValueType;
  using namespace std::string_literals;

  Json::Value root = ValueType::objectValue;
  root.setComment("// root must be a JSON object"s,
                  Json::CommentPlacement::commentBefore);

  Json::Value& textures = root["texturesToLoad"];
  textures = ValueType::arrayValue;
  textures.setComment("// 'texturesToLoad' must be a JSON array"s,
                      Json::CommentPlacement::commentBefore);

  textures.append(ValueType::stringValue);
  textures.begin()->setComment("// 'texturesToLoad' element must be a JSON string"s,
                                Json::CommentPlacement::commentBefore);

  return root;
}();

Json::Value
TextureAssetLoadList::serialize() const
{
  Json::Value json {};

  auto& jsonTextures = json["texturesToLoad"];
  jsonTextures = Json::arrayValue;

  for ( const auto& texture : texturesToLoad )
    jsonTextures.append(texture.str());

  return json;
}

void
TextureAssetLoadList::deserialize(
  entt::registry& registry,
  entt::entity entity,
  const Json::Value& json,
  const std::unordered_map <EntityId, EntityId,
                            identifier_hash>& idMap )
{
  jsonValidateObject(json, textureAssetLoadListJsonReference);

  auto& comp = registry.emplace_or_replace <TextureAssetLoadList> (entity);

  for ( const auto& texture : json["texturesToLoad"] )
    comp.texturesToLoad.insert(texture.asString());
}

} // namespace cqde::compos
