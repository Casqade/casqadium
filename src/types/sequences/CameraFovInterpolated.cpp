#include <cqde/types/sequences/CameraFovInterpolated.hpp>

#include <cqde/json_helpers.hpp>
#include <cqde/conversion/json_glm_vec4.hpp>

#include <cqde/components/Camera.hpp>

#include <entt/entity/registry.hpp>

#include <glm/gtx/compatibility.hpp>

#include <spdlog/fmt/bundled/format.h>

#include <json/value.h>

#include <imgui_bezier.hpp>


namespace cqde::types
{

const static Json::Value cameraFovInterpolatedSequenceStepJsonReference =
[]
{
  using ValueType = Json::ValueType;
  using namespace std::string_literals;

  Json::Value root = ValueType::objectValue;
  root.setComment("// CameraFovInterpolated sequence step root must be a JSON object"s,
                   Json::CommentPlacement::commentBefore);

  root["initFromCurrentFov"] = ValueType::booleanValue;
  root["initFromCurrentFov"].setComment("// 'initFromCurrentFov' must be a JSON boolean"s,
                                        Json::CommentPlacement::commentBefore);

  return root;
}();

std::string
CameraFovInterpolated::name() const
{
  return "CameraFovInterpolated";
}

void
CameraFovInterpolated::init(
  entt::registry& registry,
  const entt::entity entity )
{
  using compos::Camera;

  mInitStatus.init();

  if ( mInitFromCurrentFov == false )
    return;

  auto& cCamera = registry.get <const Camera> (entity);

  mFov.first = cCamera.fov;
}

bool
CameraFovInterpolated::execute(
  entt::registry& registry,
  const entt::entity entity )
{
  using compos::Camera;
  using TimeUtils::Duration;

  if ( mInitStatus.initialized() == false )
    init(registry, entity);

  const bool timeExpired = mTime.expired(registry);

  auto dt = mSpline.value(std::min(mTime.progress(), 1.0));

  if ( mTime.total == Duration{} )
    dt = 1.0;

  auto& cCamera = registry.get <Camera> (entity);
  cCamera.fov = glm::mix(mFov.first, mFov.second, dt);

  return timeExpired;
}

Json::Value
CameraFovInterpolated::toJson() const
{
  using namespace json_operators;

  auto json = mInitStatus.toJson();

  json << mTime.toJson();

  json["fovInitial"] = mFov.first;
  json["fovTarget"] = mFov.second;

  json["curve"] << glm::vec4{mSpline.p0(), mSpline.p1()};
  json["initFromCurrentFov"] = mInitFromCurrentFov;

  return json;
}

void
CameraFovInterpolated::fromJson(
  const Json::Value& json )
{
  using fmt::format;

  jsonValidateObject(json, cameraFovInterpolatedSequenceStepJsonReference);

  mInitStatus.fromJson(json);
  mTime.fromJson(json);

  mInitFromCurrentFov = json["initFromCurrentFov"].asBool();

  mFov.first = json["fovInitial"].asFloat();
  mFov.second = json["fovTarget"].asFloat();

  try
  {
    glm::vec4 curve {};
    curve << json["curve"];
    mSpline.setPoints({curve}, {curve.z, curve.w});
  }
  catch ( const std::exception& e )
  {
    throw std::runtime_error(
      format("'curve' parse error: {}",
             e.what()));
  }
}

} // namespace cqde::types
