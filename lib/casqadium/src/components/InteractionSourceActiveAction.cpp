#include <cqde/components/InteractionSourceActiveAction.hpp>

#include <cqde/json_helpers.hpp>

#include <entt/entity/registry.hpp>

#include <json/value.h>


namespace cqde::compos
{

const static Json::Value interactionSourceActiveActionJsonReference =
[]
{
  using ValueType = Json::ValueType;
  using namespace std::string_literals;

  Json::Value root = ValueType::objectValue;
  root.setComment("// root must be a JSON object"s,
                   Json::CommentPlacement::commentBefore);

  auto& actionId = root["actionId"];
  actionId = ValueType::stringValue;
  actionId.setComment("// 'actionId' must be a JSON string"s,
                    Json::CommentPlacement::commentBefore);

  return root;
}();

Json::Value
InteractionSourceActiveAction::serialize() const
{
  Json::Value json {};

  json["actionId"] = actionId.str();

  return json;
}

void
InteractionSourceActiveAction::deserialize(
  entt::registry& registry,
  entt::entity entity,
  const Json::Value& json,
  const std::unordered_map <EntityId, EntityId,
                            identifier_hash>& idMap )
{
  jsonValidateObject(json, interactionSourceActiveActionJsonReference);

  auto& comp = registry.emplace_or_replace <InteractionSourceActiveAction> (entity);

  comp.actionId = json["actionId"].asString();
}

} // namespace cqde::compos
