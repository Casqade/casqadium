#include <Sdl2Wrapper/Math.hpp>

#include <string>
#include <memory>
#include <forward_list>


namespace Math
{

Vector3f::Vector3f()
  : x()
  , y()
  , z()
{}

Vector3f::Vector3f( const Vector3f& other )
  : x(other.x)
  , y(other.y)
  , z(other.z)
{}

Vector3f::Vector3f(
  const float newX,
  const float newY,
  const float newZ )
  : x(newX)
  , y(newY)
  , z(newZ)
{}

Vector3f
Vector3f::operator + ( const Vector3f& other )
{
  return
  {
    this->x + other.x,
    this->y + other.y,
    this->z + other.z
  };
}

Vector3f
Vector3f::operator - ( const Vector3f& other )
{
  return
  {
    this->x - other.x,
    this->y - other.y,
    this->z - other.z
  };
}

Vector3f
Vector3f::operator * ( const Vector3f& other )
{
  return
  {
    this->x * other.x,
    this->y * other.y,
    this->z * other.z
  };
}

Vector3f
Vector3f::operator / ( const Vector3f& other )
{
  return
  {
    this->x / other.x,
    this->y / other.y,
    this->z / other.z
  };
}

}
