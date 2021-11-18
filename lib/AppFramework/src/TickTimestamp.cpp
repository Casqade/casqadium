#include <TimeUtils/Duration.hpp>
#include <AppFramework/TickTimestamp.hpp>


TickTimestamp::TickTimestamp( const TimeUtils::Duration latestTickTimestamp )
  : mLatestTickTimestamp(latestTickTimestamp)
{}

TimeUtils::Duration
TickTimestamp::latestTickTimestamp() const
{
  return mLatestTickTimestamp;
}
