#pragma once

#include <olcPGE/olcPGE_export.hpp>

#include <cstdint>


namespace olc
{
// O------------------------------------------------------------------------------O
// | olc::vX2d - A generic 2D vector type                                         |
// O------------------------------------------------------------------------------O
template <class T>
struct v2d_generic
{
  T x {};
  T y {};


  v2d_generic() = default;
  v2d_generic(const v2d_generic&) = default;
  v2d_generic& operator=(const v2d_generic&) = default;

  v2d_generic(T _x, T _y);

  T mag() const;
  T mag2() const;
  v2d_generic  norm() const;
  v2d_generic  perp() const;
  v2d_generic  floor() const;
  v2d_generic  ceil() const;
  v2d_generic  max(const v2d_generic& v) const;
  v2d_generic  min(const v2d_generic& v) const;
  v2d_generic  cart() const;
  v2d_generic  polar() const;

  T dot(const v2d_generic& rhs) const;
  T cross(const v2d_generic& rhs) const;

  v2d_generic  operator +  (const v2d_generic& rhs) const;
  v2d_generic  operator -  (const v2d_generic& rhs) const;
  v2d_generic  operator *  (const T& rhs)           const;
  v2d_generic  operator *  (const v2d_generic& rhs) const;
  v2d_generic  operator /  (const T& rhs)           const;
  v2d_generic  operator /  (const v2d_generic& rhs) const;

  v2d_generic& operator += (const v2d_generic& rhs);
  v2d_generic& operator -= (const v2d_generic& rhs);
  v2d_generic& operator *= (const T& rhs);
  v2d_generic& operator /= (const T& rhs);
  v2d_generic& operator *= (const v2d_generic& rhs);
  v2d_generic& operator /= (const v2d_generic& rhs);
  v2d_generic  operator +  () const;
  v2d_generic  operator -  () const;

  bool operator == (const v2d_generic& rhs) const;
  bool operator != (const v2d_generic& rhs) const;

  operator v2d_generic <int32_t>() const;
  operator v2d_generic <float>() const;
  operator v2d_generic <double>() const;
};

// Note: joshinils has some good suggestions here, but they are complicated to implement at this moment,
// however they will appear in a future version of PGE
template <class T> inline v2d_generic<T> operator * (const float& lhs, const v2d_generic<T>& rhs);
template <class T> inline v2d_generic<T> operator * (const double& lhs, const v2d_generic<T>& rhs);
template <class T> inline v2d_generic<T> operator * (const int& lhs, const v2d_generic<T>& rhs);

template <class T> inline v2d_generic<T> operator / (const float& lhs, const v2d_generic<T>& rhs);
template <class T> inline v2d_generic<T> operator / (const double& lhs, const v2d_generic<T>& rhs);
template <class T> inline v2d_generic<T> operator / (const int& lhs, const v2d_generic<T>& rhs);

// To stop dandistine crying...
template <class T, class U> inline bool operator < (const v2d_generic<T>& lhs, const v2d_generic<U>& rhs);
template <class T, class U> inline bool operator > (const v2d_generic<T>& lhs, const v2d_generic<U>& rhs);

extern template struct OLCPGE_EXPORT v2d_generic <int32_t>;
extern template struct OLCPGE_EXPORT v2d_generic <uint32_t>;
extern template struct OLCPGE_EXPORT v2d_generic <float>;
extern template struct OLCPGE_EXPORT v2d_generic <double>;

using vi2d = v2d_generic <int32_t>;
using vu2d = v2d_generic <uint32_t>;
using vf2d = v2d_generic <float>;
using vd2d = v2d_generic <double>;

} // namespace olc
