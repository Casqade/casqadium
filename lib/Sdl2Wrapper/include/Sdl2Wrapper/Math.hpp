#ifndef SDL_MATH_HPP
#define SDL_MATH_HPP

#include <string>
#include <memory>
#include <forward_list>


namespace Math
{

struct Vector3f
{
  float x, y, z;

  Vector3f();
  Vector3f( const Vector3f& );
  Vector3f( const float,
            const float,
            const float );

  Vector3f operator + ( const Vector3f& other );
  Vector3f operator - ( const Vector3f& other );
  Vector3f operator * ( const Vector3f& other );
  Vector3f operator / ( const Vector3f& other );
};

}

#endif
