#include <cqde/components/LightSource.hpp>

#include <cqde/assert.hpp>
#include <cqde/json_helpers.hpp>

#include <entt/entity/registry.hpp>

#include <spdlog/fmt/bundled/format.h>

#include <json/value.h>


namespace cqde::compos
{

std::string
LightSource::LightSourceTypeToString(
  const LightSource::Type type )
{
  using LightType = LightSource::Type;

  switch (type)
  {
    case LightType::Ambient:
      return "ambient";

    case LightType::Positional:
      return "positional";

    case LightType::Directional:
      return "directional";
  }

  CQDE_ASSERT_DEBUG(false, return "");
}

LightSource::Type
LightSource::LightSourceTypeFromString(
  const std::string& type )
{
  using fmt::format;
  using LightType = LightSource::Type;

  if ( type == LightSourceTypeToString(LightType::Ambient) )
    return LightType::Ambient;

  if ( type == LightSourceTypeToString(LightType::Positional) )
    return LightType::Positional;

  if ( type == LightSourceTypeToString(LightType::Directional) )
    return LightType::Directional;

  throw std::runtime_error(
    format("'{}' is not a valid light source type", type));
}

const static Json::Value lightSourceJsonReference =
[]
{
  using ValueType = Json::ValueType;
  using namespace std::string_literals;

  Json::Value root = ValueType::objectValue;
  root.setComment("// root must be a JSON object"s,
                   Json::CommentPlacement::commentBefore);

  auto& color = root["color"];
  color = ValueType::uintValue;
  color.setComment("// 'color' must be a JSON unsigned integer"s,
                    Json::CommentPlacement::commentBefore);

  auto& radius = root["radius"];
  radius = ValueType::uintValue;
  radius.setComment("// 'radius' must be a JSON float"s,
                    Json::CommentPlacement::commentBefore);

  auto& type = root["type"];
  type = ValueType::stringValue;
  type.setComment("// 'type' must be a JSON string"s,
                  Json::CommentPlacement::commentBefore);

  auto& attenuation = root["attenuation"];
  attenuation = ValueType::objectValue;
  attenuation.setComment("// 'attenuation' must be a JSON object"s,
                          Json::CommentPlacement::commentBefore);

  auto& attenuationConstant = attenuation["constant"];
  attenuationConstant = ValueType::realValue;
  attenuationConstant.setComment("// 'constant' must be a JSON float"s,
                                  Json::CommentPlacement::commentBefore);

  auto& attenuationLinear = attenuation["linear"];
  attenuationLinear = ValueType::realValue;
  attenuationLinear.setComment("// 'linear' must be a JSON float"s,
                                Json::CommentPlacement::commentBefore);

  auto& attenuationQuadratic= attenuation["quadratic"];
  attenuationQuadratic = ValueType::realValue;
  attenuationQuadratic.setComment("// 'quadratic' must be a JSON float"s,
                                  Json::CommentPlacement::commentBefore);

  return root;
}();

Json::Value
LightSource::serialize() const
{
  Json::Value json {};

  json["color"] = color.n;
  json["radius"] = radius;
  json["type"] = LightSourceTypeToString(type);

  auto& attenuationJson = json["attenuation"];

  attenuationJson["constant"] = attenuationConstant;
  attenuationJson["linear"] = attenuationLinear;
  attenuationJson["quadratic"] = attenuationQuadratic;

  return json;
}

void
LightSource::deserialize(
  entt::registry& registry,
  entt::entity entity,
  const Json::Value& json,
  const std::unordered_map <EntityId, EntityId,
                            identifier_hash>& idMap )
{
  jsonValidateObject(json, lightSourceJsonReference);

  auto& comp = registry.emplace_or_replace <LightSource> (entity);

  comp.color = json["color"].asUInt();
  comp.radius = json["radius"].asFloat();
  comp.type = LightSourceTypeFromString( json["type"].asString() );

  auto& attenuationJson = json["attenuation"];

  comp.attenuationConstant = attenuationJson["constant"].asFloat();
  comp.attenuationLinear = attenuationJson["linear"].asFloat();
  comp.attenuationQuadratic = attenuationJson["quadratic"].asFloat();
}

} // namespace cqde::compos
