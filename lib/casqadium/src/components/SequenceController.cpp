#include <cqde/components/SequenceController.hpp>

#include <cqde/json_helpers.hpp>

#include <entt/entity/registry.hpp>

#include <json/value.h>


namespace cqde::compos
{

const static Json::Value sequenceControllerJsonReference =
[]
{
  using ValueType = Json::ValueType;
  using namespace std::string_literals;

  Json::Value root = ValueType::objectValue;
  root.setComment("// JSON root must be an object"s,
                   Json::CommentPlacement::commentBefore);

  Json::Value& steps = root["steps"];
  steps = ValueType::arrayValue;
  steps.setComment("// 'steps' must be a JSON array"s,
                    Json::CommentPlacement::commentBefore);

  steps.append(ValueType::stringValue);
  steps.begin()->setComment("// 'steps' element must be a JSON string"s,
                            Json::CommentPlacement::commentBefore);

  return root;
}();

Json::Value
SequenceController::serialize() const
{
  Json::Value json {};

  auto& jsonSteps = json["steps"];
  jsonSteps = Json::arrayValue;

  for ( const auto& step : steps )
    jsonSteps.append(step.str());

  return json;
}

void
SequenceController::deserialize(
  entt::registry& registry,
  entt::entity entity,
  const Json::Value& json )
{
  using types::ControlAxis;

  jsonValidateObject(json, sequenceControllerJsonReference);

  auto& comp = registry.emplace <SequenceController> (entity);

  for ( const auto& step : json["steps"] )
    comp.steps.push_back(step.asString());
}

} // namespace cqde::compos
