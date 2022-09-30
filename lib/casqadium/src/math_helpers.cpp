#include <cqde/math_helpers.hpp>

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

} // namespace cqde
