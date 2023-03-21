#include <cqde/types/sequences/TextureTintInterpolated.hpp>

#include <cqde/json_helpers.hpp>
#include <cqde/math_helpers.hpp>

#include <cqde/conversion/json_glm_vec4.hpp>

#include <cqde/components/TextureTint.hpp>

#include <entt/entity/registry.hpp>

#include <glm/ext/matrix_common.hpp>

#include <spdlog/fmt/bundled/format.h>

#include <json/value.h>


namespace cqde::types
{

const static Json::Value textureTintInterpolatedSequenceStepJsonReference =
[]
{
  using ValueType = Json::ValueType;
  using namespace std::string_literals;

  Json::Value root = ValueType::objectValue;
  root.setComment("// TextureTintInterpolated sequence step root must be a JSON object"s,
                   Json::CommentPlacement::commentBefore);

  root["initFromTextureTint"] = ValueType::booleanValue;
  root["initFromTextureTint"].setComment("// 'initFromTextureTint' must be a JSON boolean"s,
                                          Json::CommentPlacement::commentBefore);

  return root;
}();

std::string
TextureTintInterpolated::name() const
{
  return "TextureTintInterpolated";
}

void
TextureTintInterpolated::init(
  entt::registry& registry,
  const entt::entity entity )
{
  using compos::TextureTint;

  mInitStatus.init();

  if ( mInitFromTextureTint == false )
    return;

  auto& cTextureTint = registry.get <const TextureTint> (entity);

  mTint.first =
  {
    cTextureTint.tint.r / 255.0f,
    cTextureTint.tint.g / 255.0f,
    cTextureTint.tint.b / 255.0f,
    cTextureTint.tint.a / 255.0f,
  };
}

bool
TextureTintInterpolated::execute(
  entt::registry& registry,
  const entt::entity entity )
{
  using compos::TextureTint;

  if ( mInitStatus.initialized() == false )
    init(registry, entity);

  const bool timeExpired = mTime.expired(registry);

  const auto dt = mSpline.value(std::min(mTime.progress(), 1.0));

  const auto tint = glm::mix(mTint.first, mTint.second, dt);

  auto& cTint = registry.get <TextureTint> (entity);

  cTint.tint = olc::PixelF(tint.r, tint.g, tint.b, tint.a);

  return timeExpired;
}

Json::Value
TextureTintInterpolated::toJson() const
{
  using namespace json_operators;

  auto json = mInitStatus.toJson();
  json << mTime.toJson();

  json["tintInitial"] << mTint.first;
  json["tintTarget"] << mTint.second;

  json["curve"] << glm::vec4{mSpline.p0(), mSpline.p1()};
  json["initFromTextureTint"] = mInitFromTextureTint;

  return json;
}

void
TextureTintInterpolated::fromJson(
  const Json::Value& json )
{
  using fmt::format;

  jsonValidateObject(json, textureTintInterpolatedSequenceStepJsonReference);

  mInitStatus.fromJson(json);
  mTime.fromJson(json);

  mInitFromTextureTint = json["initFromTextureTint"].asBool();

  try
  {
    mTint.first << json["tintInitial"];
  }
  catch ( const std::exception& e )
  {
    throw std::runtime_error(
      format("'tintInitial' parse error: {}",
             e.what()));
  }

  try
  {
    mTint.second << json["tintTarget"];
  }
  catch ( const std::exception& e )
  {
    throw std::runtime_error(
      format("'tintTarget' parse error: {}",
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
