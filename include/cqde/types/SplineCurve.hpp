#pragma once

#include <glm/vec2.hpp>


namespace cqde::types
{

/// Credit goes to Mozilla:
/// https://github.com/ehsan/mozilla-history/blob/e8143112b6e3f415108a89d4d08a18c46779d722/content/smil/nsSMILKeySpline.cpp

class SplineCurve
{
  glm::dvec2 mP0 {};
  glm::dvec2 mP1 {1.0f, 1.0f};

  enum { kSplineTableSize = 11 };
  double mSampleValues[kSplineTableSize];

  static constexpr double kSampleStepSize {1.0 / (kSplineTableSize - 1)};


  void calcSampleValues();
  double getTForX( const double aX ) const;
  double newtonRaphsonIterate( const double aX, double aGuessT ) const;
  double binarySubdivide( const double aX, double aA, double aB ) const;

  static double A( const double p0, const double p1);
  static double B( const double p0, const double p1 );
  static double C( const double p0 );

  static double CalcBezier( const double dt, const double p0, const double p1 );
  static double GetSlope( const double dt, const double p0, const double p1 );

public:
  SplineCurve() = default;
  SplineCurve( const glm::dvec2& p0, const glm::dvec2& p1 );

  void setPoints( const glm::dvec2& p0, const glm::dvec2& p1 );

  double value( const double dt ) const;

  glm::dvec2 p0() const;
  glm::dvec2 p1() const;
};

} // namespace cqde::types
