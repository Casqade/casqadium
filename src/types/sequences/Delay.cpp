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
  root.setComment("// Delay sequence steop root must be a JSON object"s,
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
  return mTime.expired(registry);
}

Json::Value
Delay::toJson() const
{
  return mTime.toJson();
}

void
Delay::fromJson(
  const Json::Value& json )
{
  jsonValidateObject(json, delaySequenceStepJsonReference);

  mTime.fromJson(json);
}

} // namespace cqde::types
