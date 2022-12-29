#pragma once

#include <glm/fwd.hpp>

#include <random>
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

template <typename Number,
  typename std::enable_if <sizeof(Number) <= 4, bool>::type = true>
std::mt19937
randomNumberGenerator()
{
  return std::mt19937 { std::random_device {}() };
}

template <typename Number,
  typename std::enable_if <sizeof(Number) >= 8, bool>::type = true>
std::mt19937_64
randomNumberGenerator()
{
  return std::mt19937_64 { std::random_device {}() };
}

template <typename Number,
  typename std::enable_if <std::is_integral_v <Number>, bool>::type = true>
Number
random(
  const Number min,
  const Number max )
{
  using Distribution = std::uniform_int_distribution <Number>;

  auto rng = randomNumberGenerator <Number> ();

  return Distribution{min, max} (rng);
}

template <typename Number,
  typename std::enable_if <std::is_floating_point_v <Number>, bool>::type = true>
Number
random(
  const Number min,
  const Number max )
{
  using Distribution = std::uniform_real_distribution <Number>;

  auto rng = randomNumberGenerator <Number> ();

  return Distribution{min, max} (rng);
}

} // namespace cqde
