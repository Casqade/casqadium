#include <cqde/components/audio/AudioLoop.hpp>

#include <cqde/json_helpers.hpp>

#include <entt/entity/registry.hpp>

#include <json/value.h>


namespace cqde::compos
{

const static Json::Value audioLoopJsonReference =
[]
{
  using ValueType = Json::ValueType;
  using namespace std::string_literals;

  Json::Value root = ValueType::objectValue;
  root.setComment("// root must be a JSON object"s,
                  Json::CommentPlacement::commentBefore);

  Json::Value& layers = root["layers"];
  layers = ValueType::arrayValue;
  layers.setComment("// 'layers' must be a JSON array"s,
                      Json::CommentPlacement::commentBefore);

  layers.append(ValueType::stringValue);
  layers.begin()->setComment("// 'layers' element must be a JSON string"s,
                              Json::CommentPlacement::commentBefore);

  return root;
}();

Json::Value
AudioLoop::serialize() const
{
  Json::Value json {};

  auto& jsonLayers = json["layers"];
  jsonLayers = Json::arrayValue;

  for ( const auto& layer : layers )
    jsonLayers.append(layer.source.str());

  return json;
}

void
AudioLoop::deserialize(
  entt::registry& registry,
  entt::entity entity,
  const Json::Value& json,
  const std::unordered_map <EntityId, EntityId,
                            identifier_hash>& idMap )
{
  jsonValidateObject(json, audioLoopJsonReference);

  auto& comp = registry.emplace_or_replace <AudioLoop> (entity);

  for ( const auto& layer : json["layers"] )
    comp.layers.push_back({layer.asString()});
}

} // namespace cqde::compos
