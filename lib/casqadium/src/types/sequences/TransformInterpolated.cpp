#include <cqde/types/sequences/TransformInterpolated.hpp>

#include <cqde/components/Transform.hpp>

#include <cqde/conversion/json_glm_vec4.hpp>


#include <cqde/json_helpers.hpp>
#include <cqde/math_helpers.hpp>

#include <entt/entity/registry.hpp>

#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <spdlog/fmt/bundled/format.h>

#include <json/value.h>


namespace cqde::types
{

const static Json::Value transformInterpolatedSequenceStepJsonReference =
[]
{
  using ValueType = Json::ValueType;
  using namespace std::string_literals;

  Json::Value root = ValueType::objectValue;
  root.setComment("// TransformInterpolated sequence step root must be a JSON object"s,
                   Json::CommentPlacement::commentBefore);

  root["useWorldSpace"] = ValueType::booleanValue;
  root["useWorldSpace"].setComment("// 'useWorldSpace' must be a JSON boolean"s,
                                  Json::CommentPlacement::commentBefore);

  root["initFromTransform"] = ValueType::booleanValue;
  root["initFromTransform"].setComment("// 'initFromTransform' must be a JSON boolean"s,
                                        Json::CommentPlacement::commentBefore);

  return root;
}();

std::string
TransformInterpolated::name() const
{
  return "TransformInterpolated";
}

void
TransformInterpolated::init(
  entt::registry& registry,
  const entt::entity entity )
{
  using compos::Transform;

  if ( mInitFromTransform == false )
    return;

  auto& cTransform = registry.get <const Transform> (entity);

  if ( mUseWorldSpace == true )
    mTransform.first = GetWorldMatrix(registry, entity, cTransform);

  else
    mTransform.first = cTransform.modelLocal();
}

bool
TransformInterpolated::execute(
  entt::registry& registry,
  const entt::entity entity )
{
  using compos::Transform;

  const bool timeExpired = Delay::execute(registry, entity);

  const auto dt = mSpline.value(std::min(progress(), 1.0));

  auto transform
    = cqde::interpolate(mTransform.first,
                        mTransform.second,
                        dt);

  auto& cTransform = registry.get <Transform> (entity);

  if ( mUseWorldSpace == true )
    transform = ToLocalSpace( transform, registry,
                              entity, cTransform );

  glm::vec3 skew {};
  glm::vec4 perspective {};

  glm::decompose( transform,
                  cTransform.scale,
                  cTransform.orientation,
                  cTransform.translation,
                  skew, perspective );

  return timeExpired;
}

Json::Value
TransformInterpolated::toJson() const
{
  auto json = Delay::toJson();

  json["transformInitial"][0] << mTransform.first[0];
  json["transformInitial"][1] << mTransform.first[1];
  json["transformInitial"][2] << mTransform.first[2];
  json["transformInitial"][3] << mTransform.first[3];

  json["transformTarget"][0] << mTransform.second[0];
  json["transformTarget"][1] << mTransform.second[1];
  json["transformTarget"][2] << mTransform.second[2];
  json["transformTarget"][3] << mTransform.second[3];

  json["curve"] << glm::vec4{mSpline.p0(), mSpline.p1()};
  json["useWorldSpace"] = mUseWorldSpace;
  json["initFromTransform"] = mInitFromTransform;

  return json;
}

void
TransformInterpolated::fromJson(
  const Json::Value& json )
{
  using fmt::format;

  Delay::fromJson(json);

  jsonValidateObject(json, transformInterpolatedSequenceStepJsonReference);

  mUseWorldSpace = json["useWorldSpace"].asBool();
  mInitFromTransform = json["initFromTransform"].asBool();

  try
  {
    mTransform.first[0] << json["transformInitial"][0];
    mTransform.first[1] << json["transformInitial"][1];
    mTransform.first[2] << json["transformInitial"][2];
    mTransform.first[3] << json["transformInitial"][3];
  }
  catch ( const std::exception& e )
  {
    throw std::runtime_error(
      format("'transformInitial' parse error: {}",
             e.what()));
  }

  try
  {
    mTransform.second[0] << json["transformTarget"][0];
    mTransform.second[1] << json["transformTarget"][1];
    mTransform.second[2] << json["transformTarget"][2];
    mTransform.second[3] << json["transformTarget"][3];
  }
  catch ( const std::exception& e )
  {
    throw std::runtime_error(
      format("'transformTarget' parse error: {}",
             e.what()));
  }

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
