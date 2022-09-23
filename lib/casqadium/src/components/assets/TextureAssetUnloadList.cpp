#include <cqde/components/assets/TextureAssetUnloadList.hpp>

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

  Json::Value& textures = root["texturesToUnload"];
  textures = ValueType::arrayValue;
  textures.setComment("// 'texturesToUnload' must be a JSON array"s,
                      Json::CommentPlacement::commentBefore);

  textures.append(ValueType::stringValue);
  textures.begin()->setComment("// 'texturesToUnload' element must be a JSON string"s,
                                Json::CommentPlacement::commentBefore);

  return root;
}();

Json::Value
TextureAssetUnloadList::serialize() const
{
  Json::Value json {};

  auto& jsonTextures = json["texturesToUnload"];
  jsonTextures = Json::arrayValue;

  for ( const auto& texture : texturesToUnload )
    jsonTextures.append(texture.str());

  return json;
}

void
TextureAssetUnloadList::deserialize(
  entt::registry& registry,
  entt::entity entity,
  const Json::Value& json,
  const std::unordered_map <EntityId, EntityId,
                            identifier_hash>& idMap )
{
  jsonValidateObject(json, textureAssetLoadListJsonReference);

  auto& comp = registry.emplace_or_replace <TextureAssetUnloadList> (entity);

  for ( const auto& texture : json["texturesToUnload"] )
    comp.texturesToUnload.insert(texture.asString());
}

} // namespace cqde::compos
