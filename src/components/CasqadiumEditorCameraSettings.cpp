#include <cqde/components/CasqadiumEditorCameraSettings.hpp>

#include <cqde/json_helpers.hpp>

#include <entt/entity/registry.hpp>

#include <json/value.h>


namespace cqde::compos
{

const static Json::Value casqadiumEditorCameraSettingsJsonReference =
[]
{
  using ValueType = Json::ValueType;
  using namespace std::string_literals;

  Json::Value root = ValueType::objectValue;
  root.setComment("// root must be a JSON object"s,
                   Json::CommentPlacement::commentBefore);

  auto& color = root["speed"];
  color = ValueType::realValue;
  color.setComment("// 'speed' must be a JSON float"s,
                    Json::CommentPlacement::commentBefore);

  return root;
}();

Json::Value
CasqadiumEditorCameraSettings::serialize() const
{
  Json::Value json {Json::objectValue};

  json["speed"] = speed;

  return json;
}

void
CasqadiumEditorCameraSettings::deserialize(
  entt::registry& registry,
  entt::entity entity,
  const Json::Value& json,
  const std::unordered_map <EntityId, EntityId,
                            identifier_hash>& idMap )
{
  jsonValidateObject(json, casqadiumEditorCameraSettingsJsonReference);

  auto& comp = registry.emplace_or_replace <CasqadiumEditorCameraSettings> (entity);

  comp.speed = json["speed"].asFloat();
}

} // namespace cqde::compos
