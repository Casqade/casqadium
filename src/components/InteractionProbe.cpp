#include <cqde/components/InteractionProbe.hpp>

#include <cqde/json_helpers.hpp>

#include <entt/entity/registry.hpp>

#include <json/value.h>


namespace cqde::compos
{

const static Json::Value interactionProbeJsonReference =
[]
{
  using ValueType = Json::ValueType;
  using namespace std::string_literals;

  Json::Value root = ValueType::objectValue;
  root.setComment("// root must be a JSON object"s,
                  Json::CommentPlacement::commentBefore);

  auto& listenerId = root["listenerId"];
  listenerId = ValueType::stringValue;
  listenerId.setComment("// 'listenerId' must be a JSON string"s,
                        Json::CommentPlacement::commentBefore);

  return root;
}();

Json::Value
InteractionProbe::serialize() const
{
  Json::Value json {};

  json["listenerId"] = listener.id.str();

  return json;
}

void
InteractionProbe::deserialize(
  entt::registry& registry,
  entt::entity entity,
  const Json::Value& json,
  const std::unordered_map <EntityId, EntityId,
                            identifier_hash>& idMap )
{
  jsonValidateObject(json, interactionProbeJsonReference);

  auto& comp = registry.emplace_or_replace <InteractionProbe> (entity);

  comp.listener.id = json["listenerId"].asString();
}

} // namespace cqde::compos
