#include <TimeUtils/Duration.hpp>

#include <AppFramework/RenderCommand.hpp>


RenderCommand::RenderCommand( std::function <void( const TimeUtils::Duration )> callback )
  : mCallback(callback)
{}

void
RenderCommand::draw( const TimeUtils::Duration timeFromLatestTick )
{
  if ( mCallback )
    mCallback( timeFromLatestTick );
}
