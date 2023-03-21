#include <cqde/conversion/enum_strings.hpp>

#include <cqde/common.hpp>

#include <spdlog/fmt/bundled/format.h>


namespace cqde::types
{

std::string
TransformTypeToString(
  const TransformType type )
{
  switch (type)
  {
    case TransformType::Translate:
      return "translate";

    case TransformType::Rotate:
      return "rotate";

    case TransformType::Scale:
      return "scale";
  }

  CQDE_ASSERT_DEBUG(false, return "");
}

TransformType
TransformTypeFromString(
  const std::string& type )
{
  using fmt::format;

  if ( type == TransformTypeToString(TransformType::Translate) )
    return TransformType::Translate;

  if ( type == TransformTypeToString(TransformType::Rotate) )
    return TransformType::Rotate;

  if ( type == TransformTypeToString(TransformType::Scale) )
    return TransformType::Scale;

  throw std::runtime_error(
    format("'{}' is not a valid transform type", type));
}


std::string
MathFunctionTypeToString(
  const MathFunctionType type )
{
  switch (type)
  {
    case MathFunctionType::Constant:
      return "constant";

    case MathFunctionType::Linear:
      return "linear";

    case MathFunctionType::Sine:
      return "sine";

    case MathFunctionType::Cosine:
      return "cosine";

    case MathFunctionType::Pulse:
      return "pulse";

    case MathFunctionType::Triangle:
      return "triangle";

    case MathFunctionType::Sawtooth:
      return "sawtooth";
  }

  CQDE_ASSERT_DEBUG(false, return "");
}

MathFunctionType
MathFunctionTypeFromString(
  const std::string& type )
{
  using fmt::format;

  if ( type == MathFunctionTypeToString(MathFunctionType::Constant) )
    return MathFunctionType::Constant;

  if ( type == MathFunctionTypeToString(MathFunctionType::Linear) )
    return MathFunctionType::Linear;

  if ( type == MathFunctionTypeToString(MathFunctionType::Sine) )
    return MathFunctionType::Sine;

  if ( type == MathFunctionTypeToString(MathFunctionType::Cosine) )
    return MathFunctionType::Cosine;

  if ( type == MathFunctionTypeToString(MathFunctionType::Pulse) )
    return MathFunctionType::Pulse;

  if ( type == MathFunctionTypeToString(MathFunctionType::Triangle) )
    return MathFunctionType::Triangle;

  if ( type == MathFunctionTypeToString(MathFunctionType::Sawtooth) )
    return MathFunctionType::Sawtooth;

  throw std::runtime_error(
    format("'{}' is not a valid math function type", type));
}

}