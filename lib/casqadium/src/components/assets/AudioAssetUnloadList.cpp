#include <cqde/components/assets/AudioAssetUnloadList.hpp>

#include <cqde/json_helpers.hpp>

#include <entt/entity/registry.hpp>

#include <json/value.h>


namespace cqde::compos
{

const static Json::Value audioAssetUnloadListJsonReference =
[]
{
  using ValueType = Json::ValueType;
  using namespace std::string_literals;

  Json::Value root = ValueType::objectValue;
  root.setComment("// root must be a JSON object"s,
                  Json::CommentPlacement::commentBefore);

  Json::Value& audio = root["audioToUnload"];
  audio = ValueType::arrayValue;
  audio.setComment("// 'audioToUnload' must be a JSON array"s,
                    Json::CommentPlacement::commentBefore);

  audio.append(ValueType::stringValue);
  audio.begin()->setComment("// 'audioToUnload' element must be a JSON string"s,
                            Json::CommentPlacement::commentBefore);

  return root;
}();

Json::Value
AudioAssetUnloadList::serialize() const
{
  Json::Value json {};

  auto& jsonAudio= json["audioToUnload"];
  jsonAudio = Json::arrayValue;

  for ( const auto& audio : audioToUnload )
    jsonAudio.append(audio.str());

  return json;
}

void
AudioAssetUnloadList::deserialize(
  entt::registry& registry,
  entt::entity entity,
  const Json::Value& json,
  const std::unordered_map <EntityId, EntityId,
                            identifier_hash>& idMap )
{
  jsonValidateObject(json, audioAssetUnloadListJsonReference);

  auto& comp = registry.emplace_or_replace <AudioAssetUnloadList> (entity);

  for ( const auto& texture : json["audioToUnload"] )
    comp.audioToUnload.insert(texture.asString());
}

} // namespace cqde::compos
