#include <cqde/components/audio/AudioDrivenTransform.hpp>

#include <cqde/json_helpers.hpp>

#include <cqde/conversion/enum_strings.hpp>
#include <cqde/conversion/json_glm_vec3.hpp>

#include <entt/entity/registry.hpp>

#include <spdlog/fmt/bundled/format.h>

#include <json/value.h>


namespace cqde::compos
{

const static Json::Value audioDrivenSliderJsonReference =
[]
{
  using ValueType = Json::ValueType;
  using namespace std::string_literals;

  Json::Value root = ValueType::objectValue;
  root.setComment("// root must be a JSON object"s,
                  Json::CommentPlacement::commentBefore);

  auto& sourceEntityId = root["sourceEntityId"];
  sourceEntityId = ValueType::stringValue;
  sourceEntityId.setComment("// 'sourceEntityId' must be a JSON string"s,
                            Json::CommentPlacement::commentBefore);

  auto& magnitude = root["magnitude"];
  magnitude = ValueType::realValue;
  magnitude.setComment("// 'magnitude' must be a JSON float"s,
                    Json::CommentPlacement::commentBefore);

  auto& offset = root["offset"];
  offset = ValueType::realValue;
  offset.setComment("// 'offset' must be a JSON float"s,
                  Json::CommentPlacement::commentBefore);

  auto& damp = root["damp"];
  damp = ValueType::realValue;
  damp.setComment("// 'damp' must be a JSON float"s,
                  Json::CommentPlacement::commentBefore);

  auto& operation = root["operation"];
  operation = ValueType::stringValue;
  operation.setComment("// 'operation' must be a JSON string"s,
                        Json::CommentPlacement::commentBefore);

  auto& useWorldSpace = root["useWorldSpace"];
  useWorldSpace = ValueType::booleanValue;
  useWorldSpace.setComment("// 'useWorldSpace' must be a JSON boolean"s,
                            Json::CommentPlacement::commentBefore);

  auto& ignoreBusVolume = root["ignoreBusVolume"];
  ignoreBusVolume = ValueType::booleanValue;
  ignoreBusVolume.setComment("// 'ignoreBusVolume' must be a JSON boolean"s,
                              Json::CommentPlacement::commentBefore);

  return root;
}();

Json::Value
AudioDrivenTransform::serialize() const
{
  using types::TransformTypeToString;

  Json::Value json {};

  json["sourceEntityId"] = sourceEntity.id.str();
  json["axis"] << axis;
  json["magnitude"] = magnitude;
  json["offset"] = offset;
  json["damp"] = damp;
  json["operation"] = TransformTypeToString(operation);
  json["useWorldSpace"] = useWorldSpace;
  json["ignoreBusVolume"] = ignoreBusVolume;

  return json;
}

void
AudioDrivenTransform::deserialize(
  entt::registry& registry,
  entt::entity entity,
  const Json::Value& json,
  const std::unordered_map <EntityId, EntityId,
                            identifier_hash>& idMap )
{
  using fmt::format;
  using types::TransformTypeFromString;

  jsonValidateObject(json, audioDrivenSliderJsonReference);

  auto& comp = registry.emplace_or_replace <AudioDrivenTransform> (entity);

  comp.sourceEntity.id = json["sourceEntityId"].asString();
  comp.magnitude = json["magnitude"].asFloat();
  comp.offset = json["offset"].asFloat();
  comp.damp = json["damp"].asFloat();
  comp.operation = TransformTypeFromString(json["operation"].asString());
  comp.useWorldSpace = json["useWorldSpace"].asBool();
  comp.ignoreBusVolume = json["ignoreBusVolume"].asBool();

  try
  {
    comp.axis << json["axis"];
  }
  catch ( const std::exception& e )
  {
    throw std::runtime_error(
      format("'axis' parse error: {}",
              e.what()));
  }
}

} // namespace cqde::compos
