#include <cqde/types/sequences/Delay.hpp>

#include <cqde/types/TickCurrent.hpp>

#include <cqde/json_helpers.hpp>

#include <entt/entity/registry.hpp>

#include <json/value.h>


namespace cqde::types
{

const static Json::Value delaySequenceStepJsonReference =
[]
{
  using ValueType = Json::ValueType;
  using namespace std::string_literals;

  Json::Value root = ValueType::objectValue;
  root.setComment("// Delay sequence step root must be a JSON object"s,
                   Json::CommentPlacement::commentBefore);

  root["timeElapsed"] = ValueType::realValue;
  root["timeElapsed"].setComment("// 'timeElapsed' must be a JSON float"s,
                                  Json::CommentPlacement::commentBefore);

  root["timeTotal"] = ValueType::realValue;
  root["timeTotal"].setComment("// 'timeTotal' must be a JSON float"s,
                                Json::CommentPlacement::commentBefore);

  return root;
}();

std::string
Delay::name() const
{
  return "Delay";
}

bool
Delay::execute(
  entt::registry& registry,
  const entt::entity )
{
  const auto& tickCurrent = registry.ctx().at <TickCurrent> ();

  mTime.elapsed += tickCurrent.tickInterval;

  if ( mTime.elapsed < mTime.total )
    return false;

  return true;
}

double
Delay::progress() const
{
  if ( mTime.total != Duration{} )
    return
      static_cast <double> (mTime.elapsed) /
      static_cast <double> (mTime.total);

  return static_cast <double> (mTime.elapsed);
}

Json::Value
Delay::toJson() const
{
  Json::Value json {Json::objectValue};

  json["timeElapsed"] = static_cast <double> (mTime.elapsed);
  json["timeTotal"] = static_cast <double> (mTime.total);

  return json;
}

void
Delay::fromJson(
  const Json::Value& json )
{
  jsonValidateObject(json, delaySequenceStepJsonReference);

  mTime.elapsed = json["timeElapsed"].asFloat();
  mTime.total = json["timeTotal"].asFloat();
}

} // namespace cqde::types
