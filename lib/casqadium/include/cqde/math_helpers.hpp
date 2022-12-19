#pragma once

#include <glm/fwd.hpp>

#include <vector>


namespace cqde
{

glm::mat4 interpolate( const glm::mat4&,
                       const glm::mat4&,
                       const float delta );

struct Rect
{
  float left {};
  float right {};
  float top {};
  float bottom {};
};

bool rectsIntersect( const Rect&, const Rect& );

bool pointInRect( const glm::vec2&, const Rect& );

glm::vec2 rectCenter( const Rect& );
glm::vec2 rectCenter( const glm::vec4& );

template <typename Vec,
typename std::enable_if <std::is_arithmetic_v <decltype(Vec::x)>, bool>::type = true,
typename std::enable_if <std::is_arithmetic_v <decltype(Vec::y)>, bool>::type = true>
Rect boundingBox(
  const std::vector <Vec>& vertices )
{
  Rect result
  {
    .left = vertices.front().x,
    .right = vertices.front().x,
    .top = vertices.front().y,
    .bottom = vertices.front().y,
  };

  for ( const auto& vertex : vertices )
  {
    if ( vertex.x < result.left )
      result.left = vertex.x;

    else if ( vertex.x > result.right )
      result.right = vertex.x;

    if ( vertex.y < result.top )
      result.top = vertex.y;

    else if ( vertex.y > result.bottom )
      result.bottom = vertex.y;
  }

  return result;
}

} // namespace cqde
