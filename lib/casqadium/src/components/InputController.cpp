#include <cqde/components/InputController.hpp>

#include <cqde/json_helpers.hpp>

#include <entt/entity/registry.hpp>

#include <json/value.h>


namespace cqde::compos
{

const static Json::Value inputControllerJsonReference =
[]
{
  using ValueType = Json::ValueType;
  using namespace std::string_literals;

  Json::Value root = ValueType::objectValue;
  root.setComment("// JSON root must be an object"s,
                   Json::CommentPlacement::commentBefore);

  auto& axes = root["axes"];
  axes = ValueType::objectValue;
  axes.setComment("// 'inputs' must be a JSON object"s,
                  Json::CommentPlacement::commentBefore);

  auto& axis = axes["cqde_json_anykey"];
  axis = ValueType::objectValue;
  axis.setComment("// input axis entry must be a JSON object"s,
                  Json::CommentPlacement::commentBefore);

  return root;
}();

Json::Value
InputController::serialize() const
{
  Json::Value json {};

  auto& jsonInputs = json["inputs"];
  jsonInputs = Json::objectValue;

  for ( const auto& [axisId, controlAxis] : axes )
    jsonInputs[axisId.str()] = controlAxis.toJson();

  return json;
}

void
InputController::deserialize(
  entt::registry& registry,
  entt::entity entity,
  const Json::Value& json,
  const std::unordered_map <EntityId, EntityId,
                            identifier_hash>& idMap )
{
  using types::ControlAxis;

  jsonValidateObject(json, inputControllerJsonReference);

  auto& comp = registry.emplace <InputController> (entity);

  for ( const auto& axisId : json["inputs"].getMemberNames() )
    comp.axes[axisId] = ControlAxis(json["inputs"][axisId]);
}

} // namespace cqde::compos
