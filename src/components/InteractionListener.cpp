#include <cqde/components/InteractionListener.hpp>

#include <cqde/json_helpers.hpp>

#include <entt/entity/registry.hpp>

#include <json/value.h>


namespace cqde::compos
{

const static Json::Value interactionListenerJsonReference =
[]
{
  using ValueType = Json::ValueType;
  using namespace std::string_literals;

  Json::Value root = ValueType::objectValue;
  root.setComment("// root must be a JSON object"s,
                   Json::CommentPlacement::commentBefore);

  auto& actions = root["actions"];
  actions = ValueType::objectValue;
  actions.setComment("// 'actions' must be a JSON object"s,
                      Json::CommentPlacement::commentBefore);

  auto& callbacks = actions["cqde_json_anykey"];
  callbacks = ValueType::arrayValue;
  callbacks.setComment("// 'actions' entry must be a JSON array"s,
                        Json::CommentPlacement::commentBefore);

  callbacks.append(ValueType::stringValue);
  callbacks.begin()->setComment("// 'actions' entry element must be a JSON string"s,
                                Json::CommentPlacement::commentBefore);

  return root;
}();

Json::Value
InteractionListener::serialize() const
{
  Json::Value json {};

  auto& jsonActions = json["actions"];
  jsonActions = Json::objectValue;

  for ( const auto& [actionId, callbacks] : actions )
  {
    auto& jsonCallbacks = jsonActions[actionId.str()];
    jsonCallbacks = Json::arrayValue;

    for ( const auto& callbackId : callbacks )
      jsonCallbacks.append(callbackId.str());
  }

  return json;
}

void
InteractionListener::deserialize(
  entt::registry& registry,
  entt::entity entity,
  const Json::Value& json,
  const IdMap& idMap )
{
  jsonValidateObject(json, interactionListenerJsonReference);

  auto& comp = registry.emplace_or_replace <InteractionListener> (entity);

  auto& jsonActions = json["actions"];

  for ( const auto& actionId : jsonActions.getMemberNames() )
  {
    comp.actions[actionId] = {};

    for ( const auto& callbackId : jsonActions[actionId] )
      comp.actions[actionId].push_back(callbackId.asString());
  }
}

} // namespace cqde::compos
