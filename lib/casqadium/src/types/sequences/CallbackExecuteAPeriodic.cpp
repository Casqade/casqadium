#include <cqde/types/sequences/CallbackExecuteAPeriodic.hpp>

#include <cqde/json_helpers.hpp>

#include <entt/entity/registry.hpp>

#include <json/value.h>


namespace cqde::types
{

const static Json::Value callbackExecuteAPeriodicSequenceStepJsonReference =
[]
{
  using ValueType = Json::ValueType;
  using namespace std::string_literals;

  Json::Value root = ValueType::objectValue;
  root.setComment("// CallbackExecuteAPeriodic sequence step root must be a JSON object"s,
                   Json::CommentPlacement::commentBefore);

  return root;
}();

std::string
CallbackExecuteAPeriodic::name() const
{
  return "CallbackExecuteAPeriodic";
}

bool
CallbackExecuteAPeriodic::execute(
  entt::registry& registry,
  const entt::entity entity )
{
  if ( RandomDelay::execute(registry, entity) == true )
  {
    CallbackExecute::execute(registry, entity);
    RandomDelay::init(registry, entity);
    mTime.elapsed = {};
  }

  return false;
}

Json::Value
CallbackExecuteAPeriodic::toJson() const
{
  using namespace json_operators;

  return RandomDelay::toJson() << CallbackExecute::toJson();
}

void
CallbackExecuteAPeriodic::fromJson(
  const Json::Value& json )
{
  jsonValidateObject(json, callbackExecuteAPeriodicSequenceStepJsonReference);

  RandomDelay::fromJson(json);
  CallbackExecute::fromJson(json);
}

} // namespace cqde::types
