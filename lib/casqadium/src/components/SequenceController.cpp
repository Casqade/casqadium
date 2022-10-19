#include <cqde/components/SequenceController.hpp>

#include <cqde/types/sequences/SequenceFactory.hpp>

#include <cqde/json_helpers.hpp>

#include <entt/entity/registry.hpp>

#include <spdlog/fmt/bundled/format.h>

#include <json/value.h>


namespace cqde::compos
{

const static Json::Value sequenceControllerJsonReference =
[]
{
  using ValueType = Json::ValueType;
  using namespace std::string_literals;

  Json::Value root = ValueType::objectValue;
  root.setComment("// root must be a JSON object"s,
                   Json::CommentPlacement::commentBefore);

  Json::Value& steps = root["steps"];
  steps = ValueType::arrayValue;
  steps.setComment("// 'steps' must be a JSON array"s,
                    Json::CommentPlacement::commentBefore);

  Json::Value& stepElement = steps.append(ValueType::objectValue);
  stepElement.setComment("// 'steps' element must be a JSON object"s,
                          Json::CommentPlacement::commentBefore);

  Json::Value& step = stepElement["cqde_json_anykey"];
  step = ValueType::objectValue;
  step.setComment("// step member must be a JSON object"s,
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
  {
    Json::Value stepJson {};
    stepJson[step->name()] = step->toJson();
    jsonSteps.append(stepJson);
  }

  return json;
}

void
SequenceController::deserialize(
  entt::registry& registry,
  entt::entity entity,
  const Json::Value& json,
  const std::unordered_map <EntityId, EntityId,
                            identifier_hash>& idMap )
{
  using fmt::format;
  using types::ControlAxis;
  using types::SequenceFactory;

  jsonValidateObject(json, sequenceControllerJsonReference);

  auto& comp = registry.emplace_or_replace <SequenceController> (entity);

  auto& sequenceFactory = registry.ctx().at <SequenceFactory> ();

  for ( const auto& stepJson : json["steps"] )
  {
    const auto stepName = stepJson.getMemberNames().front();

    try
    {
      const auto step = sequenceFactory.create(stepName);

      if ( step == nullptr )
        throw std::runtime_error(
          format("Unknown sequence step type"));

      step->fromJson(stepJson[stepName]);

      comp.steps.push_back(step);
    }
    catch ( const std::exception& e )
    {
      throw std::runtime_error(
        format("Failed to deserialize sequence step '{}': {}",
               stepName, e.what()));
    }
  }
}

} // namespace cqde::compos
