#ifndef SDL_MATH_HPP
#define SDL_MATH_HPP

#include <string>
#include <memory>
#include <forward_list>


namespace olc
{

struct vf2d;


struct Vector3f
{
  float x, y, z;

  Vector3f() = default;
  Vector3f( const Vector3f& ) = default;

  Vector3f( const float,
            const float,
            const float );

  Vector3f operator + ( const Vector3f& other );
  Vector3f operator - ( const Vector3f& other );
  Vector3f operator * ( const Vector3f& other );
  Vector3f operator / ( const Vector3f& other );
};

struct RectF
{
  float x, y, w, h;

  RectF() = default;
  RectF( const RectF& ) = default;

  RectF(  const float, const float,
          const float, const float );
  RectF(  const olc::vf2d,
          const olc::vf2d );

};

}

#endif
