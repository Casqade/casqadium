#include <cqde/components/assets/AudioAssetList.hpp>

#include <cqde/json_helpers.hpp>

#include <entt/entity/registry.hpp>

#include <json/value.h>


namespace cqde::compos
{

const static Json::Value audioAssetListJsonReference =
[]
{
  using ValueType = Json::ValueType;
  using namespace std::string_literals;

  Json::Value root = ValueType::objectValue;
  root.setComment("// root must be a JSON object"s,
                  Json::CommentPlacement::commentBefore);

  Json::Value& audio = root["audio"];
  audio = ValueType::arrayValue;
  audio.setComment("// 'audio' must be a JSON array"s,
                    Json::CommentPlacement::commentBefore);

  audio.append(ValueType::stringValue);
  audio.begin()->setComment("// 'audio' element must be a JSON string"s,
                            Json::CommentPlacement::commentBefore);

  return root;
}();

Json::Value
AudioAssetList::serialize() const
{
  Json::Value json {};

  auto& jsonAudio= json["audio"];
  jsonAudio = Json::arrayValue;

  for ( const auto& audio : audio )
    jsonAudio.append(audio.str());

  return json;
}

void
AudioAssetList::deserialize(
  entt::registry& registry,
  entt::entity entity,
  const Json::Value& json,
  const IdMap& idMap )
{
  jsonValidateObject(json, audioAssetListJsonReference);

  auto& comp = registry.emplace_or_replace <AudioAssetList> (entity);

  for ( const auto& audio : json["audio"] )
    comp.audio.insert(audio.asString());
}

} // namespace cqde::compos
