#include <cqde/types/sequences/CallbackExecutePeriodic.hpp>

#include <cqde/json_helpers.hpp>

#include <entt/entity/registry.hpp>

#include <json/value.h>


namespace cqde::types
{

const static Json::Value callbackExecutePeriodicSequenceStepJsonReference =
[]
{
  using ValueType = Json::ValueType;
  using namespace std::string_literals;

  Json::Value root = ValueType::objectValue;
  root.setComment("// CallbackExecutePeriodic sequence step root must be a JSON object"s,
                   Json::CommentPlacement::commentBefore);

  return root;
}();

std::string
CallbackExecutePeriodic::name() const
{
  return "CallbackExecutePeriodic";
}

bool
CallbackExecutePeriodic::execute(
  entt::registry& registry,
  const entt::entity entity )
{
  if ( Delay::execute(registry, entity) == true )
  {
    CallbackExecute::execute(registry, entity);
    mTime.elapsed = {};
  }

  return false;
}

Json::Value
CallbackExecutePeriodic::toJson() const
{
  using namespace json_operators;

  return Delay::toJson() << CallbackExecute::toJson();
}

void
CallbackExecutePeriodic::fromJson(
  const Json::Value& json )
{
  jsonValidateObject(json, callbackExecutePeriodicSequenceStepJsonReference);

  Delay::fromJson(json);
  CallbackExecute::fromJson(json);
}

} // namespace cqde::types
