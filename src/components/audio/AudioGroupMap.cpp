#include <cqde/components/audio/AudioGroupMap.hpp>

#include <cqde/json_helpers.hpp>

#include <entt/entity/registry.hpp>

#include <json/value.h>


namespace cqde::compos
{

const static Json::Value audioGroupMapJsonReference =
[]
{
  using ValueType = Json::ValueType;
  using namespace std::string_literals;

  Json::Value root = ValueType::objectValue;
  root.setComment("// root must be a JSON object"s,
                  Json::CommentPlacement::commentBefore);

  Json::Value& groups = root["groups"];
  groups = ValueType::objectValue;
  groups.setComment("// 'groups' must be a JSON object"s,
                      Json::CommentPlacement::commentBefore);

  Json::Value& group = groups["cqde_json_anykey"];
  group = ValueType::arrayValue;
  group.setComment("// 'groups' member must be a JSON array"s,
                    Json::CommentPlacement::commentBefore);

  group.append(ValueType::stringValue);
  group.begin()->setComment("// 'groups' member element must be a JSON string"s,
                            Json::CommentPlacement::commentBefore);

  return root;
}();

Json::Value
AudioGroupMap::serialize() const
{
  Json::Value json {};

  auto& jsonGroups = json["groups"];
  jsonGroups = Json::objectValue;

  for ( const auto& [groupId, group] : groups )
  {
    Json::Value jsonGroup {};
    jsonGroup = Json::arrayValue;

    for ( const auto& audioId : group )
      jsonGroup.append(audioId.str());

    jsonGroups[groupId.str()] = jsonGroup;
  }

  return json;
}

void
AudioGroupMap::deserialize(
  entt::registry& registry,
  entt::entity entity,
  const Json::Value& json,
  const IdMap& idMap )
{
  jsonValidateObject(json, audioGroupMapJsonReference);

  auto& comp = registry.emplace_or_replace <AudioGroupMap> (entity);

  auto& jsonGroups = json["groups"];

  for ( const auto& groupId : jsonGroups.getMemberNames() )
    for ( auto& group : jsonGroups[groupId] )
      comp.groups[groupId].push_back(group.asString());
}

} // namespace cqde::compos
