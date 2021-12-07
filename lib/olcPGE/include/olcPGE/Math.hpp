#ifndef SDL_MATH_HPP
#define SDL_MATH_HPP

#include <glm/vec2.hpp>


namespace olc
{

struct RectF
{
  float x, y, w, h;

  RectF() = default;
  RectF( const RectF& ) = default;

  RectF(  const float, const float,
          const float, const float );
  RectF(  const glm::vec2,
          const glm::vec2 );

};

}

#endif
