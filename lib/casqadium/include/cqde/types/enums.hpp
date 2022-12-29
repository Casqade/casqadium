#pragma once

#include <cstdint>


namespace cqde::types
{

enum class TransformType : uint8_t
{
  Translate,
  Rotate,
  Scale,
};

enum class MathFunctionType : uint8_t
{
  Constant,
  Linear,
  Sine,
  Cosine,
  Pulse,
  Triangle,
  Sawtooth,
};

} // namespace cqde::types
