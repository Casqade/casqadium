#include <cqde/types/TimeProgress.hpp>

#include <cqde/types/TickCurrent.hpp>

#include <cqde/json_helpers.hpp>

#include <entt/entity/registry.hpp>

#include <json/value.h>


namespace cqde::types
{

const static Json::Value timeProgressJsonReference =
[]
{
  using ValueType = Json::ValueType;
  using namespace std::string_literals;

  Json::Value root = ValueType::objectValue;
  root.setComment("// TimeProgress root must be a JSON object"s,
                   Json::CommentPlacement::commentBefore);

  auto& timeElapsed = root["timeElapsed"];
  timeElapsed = ValueType::realValue;
  timeElapsed.setComment("// 'timeElapsed' must be a JSON double"s,
                          Json::CommentPlacement::commentBefore);

  auto& timeTotal = root["timeTotal"];
  timeTotal = ValueType::realValue;
  timeTotal.setComment("// 'timeTotal' must be a JSON double"s,
                        Json::CommentPlacement::commentBefore);

  return root;
}();

bool
TimeProgress::expired(
  entt::registry& registry )
{
  const auto& tickCurrent = registry.ctx().at <TickCurrent> ();

  elapsed += tickCurrent.tickInterval;

  if ( elapsed < total )
    return false;

  return true;
}

double
TimeProgress::progress() const
{
  if ( total != Duration{} )
    return
      static_cast <double> (elapsed) /
      static_cast <double> (total);

  return static_cast <double> (elapsed);
}

Json::Value
TimeProgress::toJson() const
{
  Json::Value json {};

  json["timeElapsed"] = static_cast <double> (elapsed);
  json["timeTotal"] = static_cast <double> (total);

  return json;
}

void
TimeProgress::fromJson(
  const Json::Value& json )
{
  jsonValidateObject(json, timeProgressJsonReference);

  elapsed = json["timeElapsed"].asDouble();
  total = json["timeTotal"].asDouble();
}

} // namespace cqde::types
