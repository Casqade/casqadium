#include <glm/vec2.hpp>

#include <olcPGE/Math.hpp>


namespace olc
{

RectF::RectF( const float newX, const float newY,
              const float newW, const float newH )
  : x(newX)
  , y(newY)
  , w(newW)
  , h(newH)
{}

RectF::RectF( const glm::vec2 pos,
              const glm::vec2 size )
  : x(pos.x)
  , y(pos.y)
  , w(size.x)
  , h(size.y)
{}

}
