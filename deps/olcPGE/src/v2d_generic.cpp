#include <olcPGE/v2d_generic.hpp>

#include <cmath>


namespace olc
{

template <class T>
v2d_generic <T>::v2d_generic(T _x, T _y)
  : x{_x}, y{_y}
{}

template <class T>
T v2d_generic <T>::mag() const
{
  return std::sqrt(x * x + y * y);
}

template <class T>
T v2d_generic <T>::mag2() const
{
  return x * x + y * y;
}

template <class T>
v2d_generic <T> v2d_generic <T>::norm() const
{
  T r = 1 / mag();

  return
  {
    x * r,
    y * r,
  };
}

template <class T>
v2d_generic <T> v2d_generic <T>::perp() const
{
  return
  {
    -y,
    x,
  };
}

template <class T>
v2d_generic <T> v2d_generic <T>::floor() const
{
  return
  v2d_generic <T>
  (
    std::floor(x),
    std::floor(y)
  );
}

template <class T>
v2d_generic <T> v2d_generic <T>::ceil() const
{
  return
  v2d_generic <T>
  (
    std::ceil(x),
    std::ceil(y)
  );
}

template <class T>
v2d_generic <T> v2d_generic <T>::max(const v2d_generic <T>& v) const
{
  return
  v2d_generic <T>
  (
    std::max(x, v.x),
    std::max(y, v.y)
  );
}

template <class T>
v2d_generic <T> v2d_generic <T>::min(const v2d_generic <T>& v) const
{
  return
  v2d_generic <T>
  (
    std::min(x, v.x),
    std::min(y, v.y)
  );
}

template <class T>
v2d_generic <T> v2d_generic <T>::cart() const
{
  return
  v2d_generic <T>
  (
    std::cos(y) * x,
    std::sin(y) * x
  );
}

template <class T>
v2d_generic <T> v2d_generic <T>::polar() const
{
  return
  v2d_generic <T>
  (
    mag(),
    std::atan2(y, x)
  );
}

template <class T>
T v2d_generic <T>::dot(const v2d_generic<T>& rhs) const
{
  return this->x * rhs.x + this->y * rhs.y;
}

template <class T>
T v2d_generic <T>::cross(const v2d_generic<T>& rhs) const
{
  return this->x * rhs.y - this->y * rhs.x;
}

template <class T>
v2d_generic <T> v2d_generic <T>::operator + (const v2d_generic<T>& rhs) const
{
  return
  {
    this->x + rhs.x,
    this->y + rhs.y,
  };
}

template <class T>
v2d_generic <T> v2d_generic <T>::operator - (const v2d_generic<T>& rhs) const
{
  return
  {
    this->x - rhs.x,
    this->y - rhs.y,
  };
}

template <class T>
v2d_generic <T> v2d_generic <T>::operator * (const T& rhs) const
{
  return
  {
    this->x * rhs,
    this->y * rhs,
  };
}

template <class T>
v2d_generic <T> v2d_generic <T>::operator * (const v2d_generic<T>& rhs) const
{
  return
  {
    this->x * rhs.x,
    this->y * rhs.y,
  };
}

template <class T>
v2d_generic <T> v2d_generic <T>::operator / (const T& rhs) const
{
  return
  {
    this->x / rhs,
    this->y / rhs,
  };
}

template <class T>
v2d_generic <T> v2d_generic <T>::operator / (const v2d_generic<T>& rhs) const
{
  return
  {
    this->x / rhs.x,
    this->y / rhs.y,
  };
}

template <class T>
v2d_generic <T>& v2d_generic <T>::operator += (const v2d_generic<T>& rhs)
{
  this->x += rhs.x;
  this->y += rhs.y;

  return *this;
}

template <class T>
v2d_generic <T>& v2d_generic <T>::operator -= (const v2d_generic<T>& rhs)
{
  this->x -= rhs.x;
  this->y -= rhs.y;

  return *this;
}

template <class T>
v2d_generic <T>& v2d_generic <T>::operator *= (const T& rhs)
{
  this->x *= rhs;
  this->y *= rhs;

  return *this;
}

template <class T>
v2d_generic <T>& v2d_generic <T>::operator /= (const T& rhs)
{
  this->x /= rhs;
  this->y /= rhs;

  return *this;
}

template <class T>
v2d_generic <T>& v2d_generic <T>::operator *= (const v2d_generic<T>& rhs)
{
  this->x *= rhs.x;
  this->y *= rhs.y;

  return *this;
}

template <class T>
v2d_generic <T>& v2d_generic <T>::operator /= (const v2d_generic<T>& rhs)
{
  this->x /= rhs.x;
  this->y /= rhs.y;

  return *this;
}

template <class T>
v2d_generic <T> v2d_generic <T>::operator + () const
{
  return
  {
    +x,
    +y,
  };
}

template <class T>
v2d_generic <T> v2d_generic <T>::operator - () const
{
  return
  {
    -x,
    -y,
  };
}

template <class T>
bool v2d_generic <T>::operator == (const v2d_generic <T>& rhs) const
{
  return this->x == rhs.x && this->y == rhs.y;
}

template <class T>
bool v2d_generic <T>::operator != (const v2d_generic <T>& rhs) const
{
  return this->x != rhs.x || this->y != rhs.y;
}

template <class T>
v2d_generic <T>::operator v2d_generic <int32_t> () const
{
  return
  {
    static_cast <int32_t> (this->x),
    static_cast <int32_t> (this->y),
  };
}

template <class T>
v2d_generic <T>::operator v2d_generic <float> () const
{
  return
  {
    static_cast <float> (this->x),
    static_cast <float> (this->y),
  };
}

template <class T>
v2d_generic <T>::operator v2d_generic <double> () const
{
  return
  {
    static_cast <double> (this->x),
    static_cast <double> (this->y),
  };
}


template <class T>
inline v2d_generic<T> operator * (const float& lhs, const v2d_generic<T>& rhs)
{
  return
  {
    (T)(lhs * (float) rhs.x),
    (T)(lhs * (float) rhs.y),
  };
}

template<class T>
inline v2d_generic<T> operator * (const double& lhs, const v2d_generic<T>& rhs)
{
  return
  {
    (T)(lhs * (double)rhs.x),
    (T)(lhs * (double)rhs.y),
  };
}

template<class T>
inline v2d_generic<T> operator * (const int& lhs, const v2d_generic<T>& rhs)
{
  return
  {
    (T)(lhs * (int)rhs.x),
    (T)(lhs * (int)rhs.y),
  };
}

template<class T>
inline v2d_generic<T> operator / (const float& lhs, const v2d_generic<T>& rhs)
{
  return
  {
    (T)(lhs / (float)rhs.x),
    (T)(lhs / (float)rhs.y),
  };
}

template<class T>
inline v2d_generic<T> operator / (const double& lhs, const v2d_generic<T>& rhs)
{
  return
  {
    (T)(lhs / (double)rhs.x),
    (T)(lhs / (double)rhs.y),
  };
}

template<class T>
inline v2d_generic<T> operator / (const int& lhs, const v2d_generic<T>& rhs)
{
  return
  {
    (T)(lhs / (int)rhs.x),
    (T)(lhs / (int)rhs.y),
  };
}


template<class T, class U> inline
bool operator < (const v2d_generic<T>& lhs, const v2d_generic<U>& rhs)
{
  return lhs.y < rhs.y || (lhs.y == rhs.y && lhs.x < rhs.x);
}

template<class T, class U> inline
bool operator > (const v2d_generic<T>& lhs, const v2d_generic<U>& rhs)
{
  return lhs.y > rhs.y || (lhs.y == rhs.y && lhs.x > rhs.x);
}

template class
v2d_generic <int32_t>;

template class
v2d_generic <uint32_t>;

template class
v2d_generic <float>;

template class
v2d_generic <double>;

} // namespace olc
