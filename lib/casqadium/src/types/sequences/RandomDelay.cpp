#include <cqde/types/sequences/RandomDelay.hpp>

#include <cqde/json_helpers.hpp>
#include <cqde/math_helpers.hpp>

#include <entt/entity/registry.hpp>

#include <json/value.h>


namespace cqde::types
{

const static Json::Value randomDelaySequenceStepJsonReference =
[]
{
  using ValueType = Json::ValueType;
  using namespace std::string_literals;

  Json::Value root = ValueType::objectValue;
  root.setComment("// RandomDelay sequence step root must be a JSON object"s,
                   Json::CommentPlacement::commentBefore);

  auto& delayRangeMin = root["delayRangeMin"];
  delayRangeMin = ValueType::realValue;
  delayRangeMin.setComment("// 'delayRangeMin' must be a JSON double"s,
                            Json::CommentPlacement::commentBefore);

  auto& delayRangeMax = root["delayRangeMax"];
  delayRangeMax = ValueType::realValue;
  delayRangeMax.setComment("// 'delayRangeMax' must be a JSON double"s,
                            Json::CommentPlacement::commentBefore);

  return root;
}();

std::string
RandomDelay::name() const
{
  return "RandomDelay";
}

void
RandomDelay::init(
  entt::registry& registry,
  const entt::entity entity )
{
  mInitStatus.init();

  mTime.total = random(
    static_cast <double> (mDelayRange.min),
    static_cast <double> (mDelayRange.max) );
}

bool
RandomDelay::execute(
  entt::registry& registry,
  const entt::entity entity )
{
  if ( mInitStatus.initialized() == false )
    init(registry, entity);

  return Delay::execute(registry, entity);
}

Json::Value
RandomDelay::toJson() const
{
  using namespace json_operators;

  auto json = mInitStatus.toJson();

  json << Delay::toJson();

  json["delayRangeMin"] = static_cast <double> (mDelayRange.min);
  json["delayRangeMax"] = static_cast <double> (mDelayRange.max);

  return json;
}

void
RandomDelay::fromJson(
  const Json::Value& json )
{
  jsonValidateObject(json, randomDelaySequenceStepJsonReference);

  mInitStatus.fromJson(json);
  Delay::fromJson(json);

  mDelayRange.min = json["delayRangeMin"].asDouble();
  mDelayRange.max = json["delayRangeMax"].asDouble();
}

} // namespace cqde::types
