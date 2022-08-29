#include <cqde/components/TextureBuffer.hpp>

#include <cqde/json_helpers.hpp>

#include <entt/entity/registry.hpp>

#include <json/value.h>


namespace cqde::compos
{

const static Json::Value textureBufferJsonReference =
[]
{
  using ValueType = Json::ValueType;
  using namespace std::string_literals;

  Json::Value root = ValueType::objectValue;
  root.setComment("// JSON root must be an object"s,
                  Json::CommentPlacement::commentBefore);

  root["textures"] = ValueType::arrayValue;
  root["textures"].setComment("// 'textures' must be a JSON array"s,
                              Json::CommentPlacement::commentBefore);

  root["textures"].append(ValueType::stringValue);
  root["textures"].begin()->setComment("// 'textures' element must be a JSON string"s,
                                        Json::CommentPlacement::commentBefore);

  return root;
}();

Json::Value
TextureBuffer::serialize() const
{
  Json::Value json {};

  auto& jsonTextures = json["textures"];
  jsonTextures = Json::arrayValue;

  for ( const auto& texture : textures )
    jsonTextures.append(texture.str());

  return json;
}

void
TextureBuffer::deserialize(
  entt::registry& registry,
  entt::entity entity,
  const Json::Value& json )
{
  jsonValidateObject(json, textureBufferJsonReference);

  auto& comp = registry.emplace_or_replace <TextureBuffer> (entity);

  for ( const auto& texture : json["textures"] )
    comp.textures.push_back(texture.asString());

}

} // namespace cqde::compos
