#include <olcPGE/olcPixelGameEngine.hpp>
#include <olcPGE/Math.hpp>

#include <string>
#include <memory>
#include <forward_list>


namespace olc
{

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


RectF::RectF( const float newX, const float newY,
              const float newW, const float newH )
  : x(newX)
  , y(newY)
  , w(newW)
  , h(newH)
{}

RectF::RectF( const olc::vf2d pos,
              const olc::vf2d size )
  : x(pos.x)
  , y(pos.y)
  , w(size.x)
  , h(size.y)
{}

}
