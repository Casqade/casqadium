#include <TimeUtils/Duration.hpp>

#include <cmath>
//#include <unistd.h> // usleep

#define TIME_UTILS_TEST
#if defined TIME_UTILS_TEST
  #include <vector>
  #include <chrono>
  #include <thread>
#endif


namespace TimeUtils
{

Duration
Now()
{
#if defined TIME_UTILS_WIN
  LARGE_INTEGER currentTime;
  QueryPerformanceCounter( &currentTime );

#elif defined TIME_UTILS_LIN
  timespec currentTime;
  clock_gettime( CLOCK_MONOTONIC, &currentTime );

#endif

  return currentTime;
}

bool
Sleep( const Duration& duration )
{
//  Note: Linux   implementation produces 50-200us  sleep duration error
//  Note: Windows implementation produces 300-500us sleep duration error

#if defined TIME_UTILS_WIN
  const Duration timestamp = TimeUtils::Now() + duration;
  while ( TimeUtils::Now() < timestamp )
    ::Sleep(1);

#elif defined TIME_UTILS_LIN
//  const Duration timestamp = Now() + duration;
//  while ( TimeUtils::Now() < timestamp )
//    ::usleep(1000);

  timespec timeToSleep = duration;
  clock_nanosleep( CLOCK_MONOTONIC, 0, &timeToSleep, NULL );

#endif

  return true;
}

bool
SleepUntil( const Duration& timestamp )
{
//  Note: Linux   implementation produces 50-200us  sleep duration error
//  Note: Windows implementation produces 300-500us sleep duration error

#if defined TIME_UTILS_WIN
  while ( TimeUtils::Now() < timestamp )
    ::Sleep(1);

#elif defined TIME_UTILS_LIN
//  while ( TimeUtils::Now() < timestamp )
//    ::usleep(1000);

  timespec timeToSleep = timestamp;
  clock_nanosleep( CLOCK_MONOTONIC, TIMER_ABSTIME, &timeToSleep, NULL );

#endif

  return true;
}


Duration::Duration()
  : mSec()
  , mNSec()
{}

Duration::Duration( const double time )
  : mSec(static_cast <int64_t> ( std::trunc(time) ))
  , mNSec(static_cast <int64_t> ( time * NSEC_IN_SEC ) % NSEC_IN_SEC)
{
  mSec
    = static_cast <int64_t> ( std::trunc(time) );

  mNSec
    = static_cast <int64_t> ( time * NSEC_IN_SEC ) % NSEC_IN_SEC;
}

Duration::Duration( const int64_t sec,
                    const int64_t nsec )
  : mSec(sec)
  , mNSec(nsec)
{}

bool
Duration::sleep() const
{
  return TimeUtils::Sleep( *this );
}

Duration
Duration::now()
{
  return *this = TimeUtils::Now();
}

#if defined (TIME_UTILS_WIN)
Duration::Duration( const LARGE_INTEGER& time )
{
  LARGE_INTEGER perfFreq;
  QueryPerformanceFrequency( &perfFreq );

//  Note: Windows has 100 ns time stamp resolution
  mSec = time.QuadPart / perfFreq.QuadPart;
  mNSec = (time.QuadPart % perfFreq.QuadPart) * NSEC_IN_SEC / perfFreq.QuadPart;
}

Duration::operator LARGE_INTEGER () const
{
  LARGE_INTEGER perfFreq, value;

  QueryPerformanceFrequency( &perfFreq );
  value.QuadPart = mSec * perfFreq.QuadPart + mNSec * perfFreq.QuadPart / NSEC_IN_SEC;

  return value;
}

#elif defined (TIME_UTILS_LIN)
Duration::Duration( const struct timespec& time )
{
  mSec = time.tv_sec;
  mNSec = time.tv_nsec;
}

Duration::operator timespec () const
{
  timespec value;

  value.tv_sec  = mSec;
  value.tv_nsec = mNSec;

  return value;
}

#endif

Duration::operator double () const
{
  return
    mSec + mNSec / static_cast <double> ( NSEC_IN_SEC );
}

bool
Duration::operator == ( const Duration& other ) const
{
  return
  {
    this->mSec == other.mSec &&
    this->mNSec == other.mNSec
  };
}

bool
Duration::operator != ( const Duration& other ) const
{
  return !( *this == other );
}

bool
Duration::operator < ( const Duration& other ) const
{
  if ( this->mSec == other.mSec )
    return this->mNSec < other.mNSec;

  return this->mSec < other.mSec;
}

bool
Duration::operator > ( const Duration& other ) const
{
  return other < *this;
}

bool
Duration::operator <= ( const Duration& other ) const
{
  return !( *this > other );
}

bool
Duration::operator >= ( const Duration& other ) const
{
  return !( *this < other );
}

Duration
Duration::operator + ( const Duration& other ) const
{
  Duration result;

  result.mSec = this->mSec + other.mSec;
  result.mNSec = this->mNSec + other.mNSec;

  if ( result.mNSec >= NSEC_IN_SEC )
  {
    ++result.mSec;
    result.mNSec -= NSEC_IN_SEC;
  }

  return result;
}

Duration
Duration::operator - ( const Duration& other ) const
{
  Duration result;

  result.mSec = this->mSec - other.mSec;
  result.mNSec = this->mNSec - other.mNSec;

  if ( result.mNSec < 0 )
  {
    --result.mSec;
    result.mNSec += NSEC_IN_SEC;
  }

  return result;
}

Duration&
Duration::operator += ( const Duration& other )
{
  return *this = *this + other;
}

Duration&
Duration::operator -= ( const Duration& other )
{
  return *this = *this - other;
}

#ifdef TIME_UTILS_TEST
double
averageSleepTime( const TimeUtils::Duration& sleepTime,
                  const uint32_t iterations )
{
  std::vector <TimeUtils::Duration> times;
  times.reserve( iterations );

  for ( uint32_t iteration = 0;
        iteration < iterations;
        ++iteration)
  {
    auto previousTime = TimeUtils::Now();
 //   std::this_thread::sleep_for( std::chrono::duration <double> ( double(sleepTime) ) );
    if ( sleepTime.sleep() )
      times.push_back( TimeUtils::Now() - previousTime );
  }

  double result = 0.0;
  for ( auto time : times )
    result += (double) time;

  return result /= times.size();
}
#endif

} // namespace TimeUtils
