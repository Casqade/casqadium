#include <cqde/types/sequences/TransformInterpolated.hpp>

#include <cqde/components/Transform.hpp>

#include <cqde/conversion/json_glm_vec4.hpp>

#include <cqde/types/TickCurrent.hpp>

#include <cqde/json_helpers.hpp>

#include <entt/entity/registry.hpp>

#include <glm/gtx/matrix_interpolation.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <spdlog/fmt/bundled/format.h>

#include <json/value.h>

#include <imgui_bezier.hpp>


namespace cqde::types
{

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

  auto& cTransform = registry.get <Transform> (entity);

  mTransform.first = GetWorldMatrix(registry, entity, cTransform);
}

bool
TransformInterpolated::execute(
  entt::registry& registry,
  const entt::entity entity )
{
  using compos::Transform;
  using types::TickCurrent;

  if ( Delay::execute(registry, entity) == true )
    return true;

  float dt = static_cast <double> (mTime.first)
          / static_cast <double> (mTime.second);

  dt = ImGui::BezierValue(dt, glm::value_ptr(mBezierParams));

  auto transform
    = glm::interpolate( mTransform.first,
                        mTransform.second,
                        dt);

  auto& cTransform = registry.get <Transform> (entity);

  transform = ToLocalSpace( transform, registry,
                            entity, cTransform );

  glm::vec3 skew {};
  glm::vec4 perspective {};

  glm::decompose( transform,
                  cTransform.scale,
                  cTransform.orientation,
                  cTransform.translation,
                  skew, perspective );

  return false;
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

  json["bezierParams"] << mBezierParams;

  return json;
}

void
TransformInterpolated::fromJson(
  const Json::Value& json )
{
  using fmt::format;

  Delay::fromJson(json);

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
    mBezierParams << json["bezierParams"];
  }
  catch ( const std::exception& e )
  {
    throw std::runtime_error(
      format("'bezierParams' parse error: {}",
             e.what()));
  }
}

} // namespace cqde::types
