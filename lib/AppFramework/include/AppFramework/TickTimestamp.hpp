#ifndef TICK_TIMESTAMP_HPP
#define TICK_TIMESTAMP_HPP

#include <AppFramework/RenderCommand.hpp>


///
/// \brief Stores latest tick timestamp in RenderQueue
///         (Just a workaround for passing
///         latest tick timestamp to rendering thread)
///
/// \param latestTickTimestamp: timestamp of latest tick
///
class TickTimestamp : public RenderCommand
{
  TimeUtils::Duration mLatestTickTimestamp;

public:
  TickTimestamp( const TimeUtils::Duration latestTickTimestamp );
  TimeUtils::Duration latestTickTimestamp() const;
};


#endif
