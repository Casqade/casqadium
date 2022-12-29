#include <cqde/types/sequences/TransformManipulate.hpp>

#include <cqde/common.hpp>
#include <cqde/json_helpers.hpp>

#include <cqde/conversion/enum_strings.hpp>
#include <cqde/conversion/json_glm_vec3.hpp>

#include <cqde/components/Transform.hpp>
#include <cqde/types/TickCurrent.hpp>

#include <entt/entity/registry.hpp>

#include <spdlog/fmt/bundled/format.h>

#include <json/value.h>


namespace cqde::types
{

const static Json::Value transformManipulateSequenceStepJsonReference =
[]
{
  using ValueType = Json::ValueType;
  using namespace std::string_literals;

  Json::Value root = ValueType::objectValue;
  root.setComment("// TransformManipulate sequence step root must be a JSON object"s,
                   Json::CommentPlacement::commentBefore);

  auto& function = root["function"];
  function = ValueType::stringValue;
  function.setComment("// 'function' must be a JSON string"s,
                      Json::CommentPlacement::commentBefore);

  auto& operation = root["operation"];
  operation = ValueType::stringValue;
  operation.setComment("// 'operation' must be a JSON string"s,
                        Json::CommentPlacement::commentBefore);

  auto& amplitude = root["amplitude"];
  amplitude = ValueType::realValue;
  amplitude.setComment("// 'amplitude' must be a JSON double"s,
                        Json::CommentPlacement::commentBefore);

  auto& period = root["period"];
  period = ValueType::realValue;
  period.setComment("// 'period' must be a JSON double"s,
                    Json::CommentPlacement::commentBefore);

  auto& phase = root["phase"];
  phase = ValueType::realValue;
  phase.setComment("// 'phase' must be a JSON double"s,
                    Json::CommentPlacement::commentBefore);

  auto& offset = root["offset"];
  offset = ValueType::realValue;
  offset.setComment("// 'offset' must be a JSON double"s,
                    Json::CommentPlacement::commentBefore);

  auto& useWorldSpace = root["useWorldSpace"];
  useWorldSpace = ValueType::booleanValue;
  useWorldSpace.setComment("// 'useWorldSpace' must be a JSON boolean"s,
                            Json::CommentPlacement::commentBefore);

  auto& transformRelative = root["transformRelative"];
  transformRelative = ValueType::booleanValue;
  transformRelative.setComment("// 'transformRelative' must be a JSON boolean"s,
                                Json::CommentPlacement::commentBefore);

  return root;
}();

std::string
TransformManipulate::name() const
{
  return "TransformManipulate";
}

bool
TransformManipulate::execute(
  entt::registry& registry,
  const entt::entity entity )
{
  using compos::Transform;
  using TimeUtils::Duration;

  const bool timeExpired =
    mTime.expired(registry) == true &&
    mTime.total != Duration{};

  const float factor = value(static_cast <double> (mTime.elapsed));

  const auto& tickCurrent = registry.ctx().at <TickCurrent> ();
  const auto dt = static_cast <float> (tickCurrent.tickInterval);

  auto& cTransform = registry.get <Transform> (entity);

  switch (mOperation)
  {
    case TransformType::Translate:
    {
      auto translation = mAxis * factor;

      if ( mUseWorldSpace == true )
        translation = ToLocalSpace(translation, registry, entity, cTransform);

      if ( mTransformRelative == true )
        cTransform.translation += translation * dt;
      else
        cTransform.translation =
        {
          glm::epsilonEqual(mAxis.x, 0.0f, glm::epsilon <float> () ) == false
            ? translation.x : cTransform.translation.x,
          glm::epsilonEqual(mAxis.y, 0.0f, glm::epsilon <float> () ) == false
            ? translation.y : cTransform.translation.y,
          glm::epsilonEqual(mAxis.z, 0.0f, glm::epsilon <float> () ) == false
            ? translation.z : cTransform.translation.z,
        };

      return timeExpired;
    }

    case TransformType::Scale:
    {
      auto scale = mAxis * factor;

      if ( mUseWorldSpace == true )
        scale = ToLocalSpace(scale, registry, entity, cTransform);

      if ( mTransformRelative == true )
        cTransform.scale += scale * dt;
      else
        cTransform.scale =
        {
          glm::epsilonEqual(mAxis.x, 0.0f, glm::epsilon <float> () ) == false
            ? scale.x : cTransform.scale.x,
          glm::epsilonEqual(mAxis.y, 0.0f, glm::epsilon <float> () ) == false
            ? scale.y : cTransform.scale.y,
          glm::epsilonEqual(mAxis.z, 0.0f, glm::epsilon <float> () ) == false
            ? scale.z : cTransform.scale.z,
        };

      return timeExpired;
    }

    case TransformType::Rotate:
    {
      glm::quat rotation {};

      if ( mTransformRelative == true )
        rotation = glm::angleAxis(factor * dt, mAxis);
      else
        rotation = glm::angleAxis(factor, mAxis);

      if ( mUseWorldSpace == true )
        rotation = ToLocalSpace(rotation, registry, entity, cTransform);

      if ( mTransformRelative == true )
        cTransform.orientation = glm::normalize(cTransform.orientation * rotation);
      else
        cTransform.orientation = rotation;

      return timeExpired;
    }

    default:
      CQDE_ASSERT_DEBUG(false, return true);
  }

  return true;
}

double
TransformManipulate::value(
  const double input ) const
{
  const auto angularFrequncy = 2.0 * glm::pi <float> () / mPeriod;

  switch (mFunction)
  {
    case MathFunctionType::Constant:
      return mOffset + mAmplitude;

    case MathFunctionType::Linear:
      return mOffset + mAmplitude * (input + mPhase);

    case MathFunctionType::Sine:
      return mOffset + mAmplitude * std::sin(std::fma(angularFrequncy, input, mPhase));

    case MathFunctionType::Cosine:
      return mOffset + mAmplitude * std::cos(std::fma(angularFrequncy, input, mPhase));

    case MathFunctionType::Pulse:
      return std::copysign(mAmplitude, std::sin(std::fma(angularFrequncy, input, mPhase)));

    case MathFunctionType::Triangle:
//      return  mAmplitude * 4.0 / mPeriod * ( std::fmod(input - mPeriod * 0.25, mPeriod)
//                                             - mPeriod * 0.5 )
//              - mAmplitude;
      return  2.0 * mAmplitude / glm::pi <float> ()
            * std::asin(std::sin(2.0 * glm::pi <float> () / mPeriod * input))
            + mPhase;

    case MathFunctionType::Sawtooth:
      return mOffset + mAmplitude * (std::fmod(input + mPhase, mPeriod) - mPeriod * 0.5);

    default:
      CQDE_ASSERT_DEBUG(false, return 0.0);
  }
}

Json::Value
TransformManipulate::toJson() const
{
  auto json = mTime.toJson();

  json["axis"] << mAxis;
  json["function"] = MathFunctionTypeToString(mFunction);
  json["operation"] = TransformTypeToString(mOperation);

  json["amplitude"] = mAmplitude;
  json["period"] = mPeriod;
  json["phase"] = mPhase;
  json["offset"] = mOffset;

  json["useWorldSpace"] = mUseWorldSpace;
  json["transformRelative"] = mTransformRelative;

  return json;
}

void
TransformManipulate::fromJson(
  const Json::Value& json )
{
  using fmt::format;

  jsonValidateObject(json, transformManipulateSequenceStepJsonReference);

  mTime.fromJson(json);

  try
  {
    mAxis << json["axis"];
  }
  catch ( const std::exception& e )
  {
    throw std::runtime_error(
      format("'axis' parse error: {}",
             e.what()));
  }

  mFunction = MathFunctionTypeFromString(json["function"].asString());
  mOperation = TransformTypeFromString(json["operation"].asString());

  mAmplitude = json["amplitude"].asDouble();
  mPeriod = json["period"].asDouble();
  mPhase = json["phase"].asDouble();
  mOffset = json["offset"].asDouble();

  mUseWorldSpace = json["useWorldSpace"].asBool();
  mTransformRelative = json["transformRelative"].asBool();
}

} // namespace cqde::types
