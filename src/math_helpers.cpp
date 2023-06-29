#include <cqde/math_helpers.hpp>

#include <cqde/assert.hpp>
#include <cqde/types/enums.hpp>

#include <glm/vec2.hpp>
#include <glm/gtc/quaternion.hpp>


namespace cqde
{

glm::mat4
interpolate(
  const glm::mat4& source,
  const glm::mat4& target,
  const float delta )
{
  const auto translationSource {source[3]};
  const auto translationTarget {target[3]};

  const glm::quat orientationSource {source};
  const glm::quat orientationTarget {target};

  const auto orientationCurrent
    = glm::slerp( orientationSource,
                  orientationTarget,
                  delta );

  glm::mat4 transform {orientationCurrent};

  transform[3] = glm::mix(translationSource,
                          translationTarget,
                          delta);

  return transform;
}

double
waveformValue(
  const types::MathFunctionType function,
  const double input,
  const double offset,
  const double amplitude,
  const double period,
  const double phase )
{
  using types::MathFunctionType;

  const auto angularFrequncy = 2.0 * glm::pi <float> () / period;

  switch (function)
  {
    case MathFunctionType::Constant:
      return offset + amplitude;

    case MathFunctionType::Linear:
      return offset + amplitude * (input + phase);

    case MathFunctionType::Sine:
      return offset + amplitude * std::sin(std::fma(angularFrequncy, input, phase));

    case MathFunctionType::Cosine:
      return offset + amplitude * std::cos(std::fma(angularFrequncy, input, phase));

    case MathFunctionType::Pulse:
      return offset + std::copysign(amplitude, std::sin(std::fma(angularFrequncy, input, phase)));

    case MathFunctionType::Triangle:
//      return  amplitude * 4.0 / period * ( std::fmod(input - period * 0.25, period)
//                                            - period * 0.5 )
//              - amplitude;
      return  2.0 * amplitude / glm::pi <float> ()
            * std::asin(std::sin(2.0 * glm::pi <float> () / period * input))
            + phase;

    case MathFunctionType::Sawtooth:
      return offset + amplitude * (std::fmod(input + phase, period) - period * 0.5);

    default:
      CQDE_ASSERT_DEBUG(false, return 0.0);
  }
}


bool rectsIntersect(
  const Rect& first,
  const Rect& second )
{
  return
    first.left < second.right &&
    first.right > second.left &&
    first.top < second.bottom &&
    first.bottom > second.top;
}

bool
pointInRect(
  const glm::vec2& p,
  const Rect& rect )
{
  return
    p.x > rect.left &&
    p.x < rect.right &&
    p.y > rect.top &&
    p.y < rect.bottom;
}

glm::vec2
rectCenter(
  const Rect& rect )
{
  return
  {
    rect.left + (rect.right - rect.left) * 0.5f,
    rect.top + (rect.bottom - rect.top) * 0.5f,
  };
}

glm::vec2
rectCenter(
  const glm::vec4& rect )
{
  return
  {
    rect.x + rect.z * 0.5f,
    rect.y + rect.w * 0.5f,
  };
}

} // namespace cqde
