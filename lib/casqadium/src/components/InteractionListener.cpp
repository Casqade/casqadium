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

  auto& callbacks = root["callbacks"];
  callbacks = ValueType::arrayValue;
  callbacks.setComment("// 'callbacks' must be a JSON array"s,
                        Json::CommentPlacement::commentBefore);

  callbacks.append(ValueType::stringValue);
  callbacks.begin()->setComment("// 'callbacks' element must be a JSON string"s,
                                Json::CommentPlacement::commentBefore);

  return root;
}();

Json::Value
InteractionListener::serialize() const
{
  Json::Value json {};

  auto& jsonCallbacks = json["callbacks"];
  jsonCallbacks = Json::arrayValue;

  for ( const auto& callback : callbacks )
    jsonCallbacks.append(callback.str());

  return json;
}

void
InteractionListener::deserialize(
  entt::registry& registry,
  entt::entity entity,
  const Json::Value& json,
  const std::unordered_map <EntityId, EntityId,
                            identifier_hash>& idMap )
{
  jsonValidateObject(json, interactionListenerJsonReference);

  auto& comp = registry.emplace_or_replace <InteractionListener> (entity);

  for ( const auto& callback : json["callbacks"] )
    comp.callbacks.push_back(callback.asString());
}

} // namespace cqde::compos
