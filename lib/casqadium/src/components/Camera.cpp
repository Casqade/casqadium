#include <cqde/components/Camera.hpp>
#include <cqde/components/Transform.hpp>

#include <cqde/json_helpers.hpp>

#include <cqde/conversion/json_glm_vec4.hpp>

#include <entt/entity/registry.hpp>

#include <spdlog/fmt/bundled/format.h>

#include <json/value.h>


namespace cqde::compos
{

Camera::Camera(
  const glm::vec4& _viewport,
  const Projection _projectionType )
  : viewport(_viewport)
  , projectionType(_projectionType)
{}

glm::vec4
Camera::viewportScaled() const
{
  glm::vec4 result {};

  const float viewportW = olc::renderer->ptrPGE->GetWindowSize().x;
  const float viewportH = olc::renderer->ptrPGE->GetWindowSize().y;

  result.x = viewport.x * viewportW;
  result.y = viewport.y * viewportH;
  result.z = viewport.z * viewportW;
  result.w = viewport.w * viewportH;

  return result;
}

glm::mat4
Camera::projMatrix() const
{
  const glm::vec4 viewport = viewportScaled();

  if ( projectionType == Projection::Perspective )
    return glm::perspectiveFov( fov, viewport.z, viewport.w,
                                zRange.first, zRange.second);

  return glm::ortho(-viewport.z * zoom, viewport.z * zoom,
                    -viewport.w * zoom, viewport.w * zoom,
                      zRange.first, zRange.second );
}

glm::mat4
Camera::viewMatrix(
  entt::registry&   registry,
  const SceneNode&  cSceneNode,
  const Transform&  cTransform ) const
{
  return glm::inverse(GetWorldMatrix(registry, cTransform, cSceneNode));
}

const static Json::Value cameraJsonReference =
[]
{
  using ValueType = Json::ValueType;
  using namespace std::string_literals;

  Json::Value root = ValueType::objectValue;
  root.setComment("// JSON root must be an object"s,
                  Json::CommentPlacement::commentBefore);

  root["viewport"] = ValueType::arrayValue;
  root["viewport"].setComment("// 'viewport' must be a JSON array"s,
                              Json::CommentPlacement::commentBefore);

  root["viewport"].append(ValueType::realValue);
  root["viewport"].begin()->setComment("// 'viewport' element must be a JSON float"s,
                                        Json::CommentPlacement::commentBefore);

  root["z-range"] = ValueType::arrayValue;
  root["z-range"].setComment("// 'z-range' must be a JSON array"s,
                              Json::CommentPlacement::commentBefore);

  root["z-range"].append(ValueType::realValue);
  root["z-range"].begin()->setComment("// 'z-range' element must be a JSON float"s,
                                      Json::CommentPlacement::commentBefore);

  root["fov"] = ValueType::realValue;
  root["fov"].setComment("// 'fov' must be a JSON float"s,
                          Json::CommentPlacement::commentBefore);

  root["layer"] = ValueType::intValue;
  root["layer"].setComment("// 'layer' must be a JSON integer"s,
                          Json::CommentPlacement::commentBefore);

  root["projection"] = ValueType::stringValue;
  root["projection"].setComment("// 'projection' must be a JSON string"s,
                                Json::CommentPlacement::commentBefore);

//  todo: render, texture & lighting modes

  return root;
}();

Json::Value
Camera::serialize() const
{
  Json::Value json {};

  json["viewport"] << viewport;
  json["z-range"].append(zRange.first);
  json["z-range"].append(zRange.second);

  if ( projectionType == Projection::Perspective )
    json["fov"] = glm::degrees(fov);
  else
    json["fov"] = fov;

  json["layer"] = layer;

  json["projection"] = projectionType == Projection::Perspective
                       ? "perspective"
                       : "orthographic";

//  todo: render, texture & lighting modes

  return json;
}

void
Camera::deserialize(
  entt::registry& registry,
  entt::entity entity,
  const Json::Value& json )
{
  using fmt::format;

  jsonValidateObject(json, cameraJsonReference);

  const auto projection = json["projection"].asString();

  if ( projection != "perspective" &&
       projection != "orthographic" )
    throw std::runtime_error(
      format("'projection' value must be either "
             "'{}' or '{}' (given '{}')",
             "perspective", "orthographic", projection));

  if ( json["z-range"].size() != 2 )
    throw std::runtime_error(
      "'z-range' must contain 2 elements");

  auto& comp = registry.emplace <Camera> (entity);

  comp.projectionType = projection == "perspective"
                        ? Projection::Perspective
                        : Projection::Orthographic;

  if ( comp.projectionType == Projection::Perspective )
    comp.fov = glm::radians(json["fov"].asFloat());
  else
    comp.fov = json["fov"].asFloat();

  comp.layer = json["layer"].asInt();

  comp.viewport << json["viewport"];
  comp.zRange.first = json["z-range"][0].asFloat();
  comp.zRange.second = json["z-range"][1].asFloat();


//  todo: render, texture & lighting modes
}

} // namespace cqde::compos
