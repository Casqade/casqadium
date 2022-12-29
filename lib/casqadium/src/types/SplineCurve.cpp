#include <cqde/types/SplineCurve.hpp>

#include <cmath>

#define NEWTON_ITERATIONS          4
#define NEWTON_MIN_SLOPE           0.02
#define SUBDIVISION_PRECISION      0.0000001
#define SUBDIVISION_MAX_ITERATIONS 10

#define SAMPLES_SIZE 11
#define SAMPLES_STEP (1.0f / (SAMPLES_SIZE - 1))


namespace cqde::types
{

SplineCurve::SplineCurve(
  const glm::dvec2& p0,
  const glm::dvec2& p1 )
{
  setPoints(p0, p1);
}

void
SplineCurve::setPoints(
  const glm::dvec2& p0,
  const glm::dvec2& p1 )
{
  mP0 = p0;
  mP1 = p1;

  calcSampleValues();
}

double
SplineCurve::value(
  const double dt ) const
{
  if (  mP0.x == mP0.y && mP1.x == mP1.y )
    return dt;

  return CalcBezier( getTForX(dt), mP0.y, mP1.y );
}

void
SplineCurve::calcSampleValues()
{
  if (mP0.x == mP0.y && mP1.x == mP1.y )
    return;

  for ( uint32_t i = 0; i < kSplineTableSize; ++i )
    mSampleValues[i] = CalcBezier(double(i) * kSampleStepSize, mP0.x, mP1.x);
}

double
SplineCurve::CalcBezier(
  const double dt,
  const double p0,
  const double p1 )
{
  // use Horner's scheme to evaluate the Bezier polynomial
  return ( (A(p0, p1) * dt + B(p0, p1)) * dt + C(p0) ) * dt;
}

double
SplineCurve::GetSlope(
  const double dt,
  const double p0,
  const double p1 )
{
  return  3.0 * A(p0, p1) * dt * dt
        + 2.0 * B(p0, p1) * dt
        + C(p0);
}

double
SplineCurve::getTForX(
  const double aX ) const
{
  // Find interval where t lies
  double intervalStart = 0.0;
  const double* currentSample = &mSampleValues[1];
  const double* const lastSample = &mSampleValues[kSplineTableSize - 1];

  for ( ; currentSample != lastSample && *currentSample <= aX;
        ++currentSample )
    intervalStart += kSampleStepSize;

  --currentSample; // t now lies between *currentSample and *currentSample+1

  // Interpolate to provide an initial guess for t
  double dist = (aX - *currentSample) /
                (*(currentSample+1) - *currentSample);
  double guessForT = intervalStart + dist * kSampleStepSize;

  // Check the slope to see what strategy to use. If the slope is too small
  // Newton-Raphson iteration won't converge on a root so we use bisection
  // instead.
  double initialSlope = GetSlope(guessForT, mP0.x, mP1.x);

  if ( initialSlope >= NEWTON_MIN_SLOPE )
    return newtonRaphsonIterate(aX, guessForT);

  else if ( initialSlope == 0.0 )
    return guessForT;

  else
    return binarySubdivide(aX, intervalStart, intervalStart + kSampleStepSize);
}

double
SplineCurve::newtonRaphsonIterate(double aX, double aGuessT) const
{
  // Refine guess with Newton-Raphson iteration
  for ( uint32_t i = 0; i < NEWTON_ITERATIONS; ++i )
  {
    // We're trying to find where f(t) = aX,
    // so we're actually looking for a root for: CalcBezier(t) - aX
    double currentX = CalcBezier(aGuessT, mP0.x, mP1.x) - aX;
    double currentSlope = GetSlope(aGuessT, mP0.x, mP1.x);

    if (currentSlope == 0.0)
      return aGuessT;

    aGuessT -= currentX / currentSlope;
  }

  return aGuessT;
}

double
SplineCurve::binarySubdivide(
  const double aX,
  double aA,
  double aB ) const
{
  double currentX;
  double currentT;
  uint32_t i = 0;

  do
  {
    currentT = aA + (aB - aA) / 2.0;
    currentX = CalcBezier(currentT, mP0.x, mP1.x) - aX;

    if (currentX > 0.0)
      aB = currentT;

    else
      aA = currentT;

  } while ( std::fabs(currentX) > SUBDIVISION_PRECISION &&
            ++i < SUBDIVISION_MAX_ITERATIONS );

  return currentT;
}

double SplineCurve::A(
  const double p0,
  const double p1 )
{
  return 1.0 - 3.0 * p1 + 3.0 * p0;
}

double SplineCurve::B(
  const double p0,
  const double p1 )
{
  return 3.0 * p1 - 6.0 * p0;
}

double SplineCurve::C(
  const double p0 )
{
  return 3.0 * p0;
}

glm::dvec2 SplineCurve::p0() const
{
  return mP0;
}

glm::dvec2 SplineCurve::p1() const
{
  return mP1;
}

} // namespace cqde::types
