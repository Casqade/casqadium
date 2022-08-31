#pragma once

#include <glm/fwd.hpp>

#include <vector>


namespace cqde
{

struct Rect
{
  float left {};
  float right {};
  float top {};
  float bottom {};
};

bool rectsIntersect( const Rect&, const Rect& );

bool pointInRect( const glm::vec2&, const Rect& );

} // namespace cqde
