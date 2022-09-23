#include <cqde/components/assets/AudioAssetLoadList.hpp>

#include <cqde/json_helpers.hpp>

#include <entt/entity/registry.hpp>

#include <json/value.h>


namespace cqde::compos
{

const static Json::Value audioAssetLoadListJsonReference =
[]
{
  using ValueType = Json::ValueType;
  using namespace std::string_literals;

  Json::Value root = ValueType::objectValue;
  root.setComment("// root must be a JSON object"s,
                  Json::CommentPlacement::commentBefore);

  Json::Value& audio = root["audioToLoad"];
  audio = ValueType::arrayValue;
  audio.setComment("// 'audioToLoad' must be a JSON array"s,
                    Json::CommentPlacement::commentBefore);

  audio.append(ValueType::stringValue);
  audio.begin()->setComment("// 'audioToLoad' element must be a JSON string"s,
                            Json::CommentPlacement::commentBefore);

  return root;
}();

Json::Value
AudioAssetLoadList::serialize() const
{
  Json::Value json {};

  auto& jsonAudio= json["audioToLoad"];
  jsonAudio = Json::arrayValue;

  for ( const auto& audio : audioToLoad )
    jsonAudio.append(audio.str());

  return json;
}

void
AudioAssetLoadList::deserialize(
  entt::registry& registry,
  entt::entity entity,
  const Json::Value& json,
  const std::unordered_map <EntityId, EntityId,
                            identifier_hash>& idMap )
{
  jsonValidateObject(json, audioAssetLoadListJsonReference);

  auto& comp = registry.emplace_or_replace <AudioAssetLoadList> (entity);

  for ( const auto& texture : json["audioToLoad"] )
    comp.audioToLoad.insert(texture.asString());
}

} // namespace cqde::compos
