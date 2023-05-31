#include <cqde/components/audio/Audio3dParams.hpp>

#include <cqde/json_helpers.hpp>
#include <cqde/conversion/soloud_string.hpp>

#include <entt/entity/registry.hpp>

#include <json/value.h>


namespace cqde::compos
{

const static Json::Value audio3dParamsJsonReference =
[]
{
  using ValueType = Json::ValueType;
  using namespace std::string_literals;

  Json::Value root = ValueType::objectValue;
  root.setComment("// root must be a JSON object"s,
                  Json::CommentPlacement::commentBefore);

  Json::Value& attenuation = root["attenuation"];
  attenuation = ValueType::objectValue;
  attenuation.setComment("// 'attenuation' must be a JSON object"s,
                          Json::CommentPlacement::commentBefore);

  Json::Value& model = attenuation["model"];
  model = ValueType::stringValue;
  model.setComment("// 'model' must be a JSON string"s,
                    Json::CommentPlacement::commentBefore);

  Json::Value& rolloff = attenuation["rolloff"];
  rolloff = ValueType::realValue;
  rolloff.setComment("// 'rolloff' must be a JSON float"s,
                      Json::CommentPlacement::commentBefore);

  Json::Value& distanceMin = attenuation["distanceMin"];
  distanceMin = ValueType::realValue;
  distanceMin.setComment("// 'distanceMin' must be a JSON float"s,
                          Json::CommentPlacement::commentBefore);

  Json::Value& distanceMax = attenuation["distanceMax"];
  distanceMax = ValueType::realValue;
  distanceMax.setComment("// 'distanceMax' must be a JSON float"s,
                          Json::CommentPlacement::commentBefore);

  Json::Value& dopplerFactor = root["dopplerFactor"];
  dopplerFactor = ValueType::realValue;
  dopplerFactor.setComment("// 'dopplerFactor' must be a JSON float"s,
                            Json::CommentPlacement::commentBefore);

  return root;
}();

Json::Value
Audio3dParams::serialize() const
{
  Json::Value json {};

  json["dopplerFactor"] = dopplerFactor;

  auto& jsonAttenuation = json["attenuation"];
  jsonAttenuation["model"] = SoLoudAttenuationModelStringConverter{}.toString(attenuation.model);
  jsonAttenuation["rolloff"] = attenuation.rolloff;
  jsonAttenuation["distanceMin"] = attenuation.distance.min;
  jsonAttenuation["distanceMax"] = attenuation.distance.max;

  return json;
}

void
Audio3dParams::deserialize(
  entt::registry& registry,
  entt::entity entity,
  const Json::Value& json,
  const std::unordered_map <EntityId, EntityId,
                            identifier_hash>& idMap )
{
  jsonValidateObject(json, audio3dParamsJsonReference);

  auto& comp = registry.emplace_or_replace <Audio3dParams> (entity);

  comp.dopplerFactor = json["dopplerFactor"].asFloat();

  auto& jsonAttenuation = json["attenuation"];
  comp.attenuation.model = SoLoudAttenuationModelStringConverter{}.toModel(jsonAttenuation["model"].asString());
  comp.attenuation.rolloff= jsonAttenuation["rolloff"].asFloat();
  comp.attenuation.distance.min = jsonAttenuation["distanceMin"].asFloat();
  comp.attenuation.distance.max = jsonAttenuation["distanceMax"].asFloat();
}

} // namespace cqde::compos
