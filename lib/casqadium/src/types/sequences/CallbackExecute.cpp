#include <cqde/types/sequences/CallbackExecute.hpp>

#include <cqde/json_helpers.hpp>
#include <cqde/types/CallbackManager.hpp>

#include <entt/entity/registry.hpp>

#include <json/value.h>


namespace cqde::types
{

const static Json::Value callbackExecuteSequenceStepJsonReference =
[]
{
  using ValueType = Json::ValueType;
  using namespace std::string_literals;

  Json::Value root = ValueType::objectValue;
  root.setComment("// CallbackExecute sequence step root must be a JSON object"s,
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

std::string
CallbackExecute::name() const
{
  return "Execute";
}

bool
CallbackExecute::execute(
  entt::registry& registry,
  const entt::entity entity )
{
  auto& callbackManager = registry.ctx().at <CallbackManager> ();

  for ( const auto& callback : mCallbacks )
    callbackManager.execute(callback, registry, {entity});

  return true;
}

Json::Value
CallbackExecute::toJson() const
{
  Json::Value json {};

  auto& jsonCallbacks = json["callbacks"];
  jsonCallbacks = Json::arrayValue;

  for ( const auto& callback : mCallbacks )
    jsonCallbacks.append(callback.str());

  return json;
}

void
CallbackExecute::fromJson(
  const Json::Value& json )
{
  jsonValidateObject(json, callbackExecuteSequenceStepJsonReference);

  for ( const auto& callback : json["callbacks"] )
    mCallbacks.push_back(callback.asString());
}

} // namespace cqde::types
