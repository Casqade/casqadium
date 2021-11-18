#ifndef TIME_UTILS_DURATION_HPP
#define TIME_UTILS_DURATION_HPP

#include <cstdint>

#if defined(__APPLE__) || defined(__MACH__)
  static_assert ( true, "TimeUtils is not compatible with OS X" );

#elif defined(_WIN32)
  #include <windows.h>

  #ifndef TIME_UTILS_WIN
    #define TIME_UTILS_WIN
  #endif

#elif defined(__linux__)
  #include <time.h>

  #ifndef TIME_UTILS_LIN
    #define TIME_UTILS_LIN
  #endif

#else
  static_assert ( true, "TimeUtils is not compatible with your OS" );

#endif


namespace TimeUtils
{

const static int64_t NSEC_IN_SEC = 1'000'000'000;


class Duration;

Duration Now();
bool Sleep( const Duration& duration );
bool SleepUntil( const Duration& timeStamp );

class Duration
{
  int64_t mSec;
  int64_t mNSec;

public:
  Duration();
  Duration( const double time );
  Duration( const int64_t sec,
            const int64_t nsec );

  bool sleep() const;
  Duration now();

#if defined (TIME_UTILS_WIN)
  Duration( const LARGE_INTEGER& time );
  operator LARGE_INTEGER () const;

#elif defined (TIME_UTILS_LIN)
  Duration( const timespec& time );
  operator timespec () const;

#endif
  explicit operator double () const;

  bool operator == ( const Duration& other) const;
  bool operator != ( const Duration& other) const;
  bool operator < ( const Duration& other) const;
  bool operator > ( const Duration& other) const;
  bool operator <= ( const Duration& other) const;
  bool operator >= ( const Duration& other) const;

  Duration operator + ( const Duration& other ) const;
  Duration operator - ( const Duration& other ) const;

  Duration& operator += ( const Duration& other );
  Duration& operator -= ( const Duration& other );
};

double
averageSleepTime( const TimeUtils::Duration& sleepTime,
                  const uint32_t iterations );

} // namespace TimeUtils


#endif
