#include <cqde/math_helpers.hpp>

#include <glm/vec2.hpp>


namespace cqde
{

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
