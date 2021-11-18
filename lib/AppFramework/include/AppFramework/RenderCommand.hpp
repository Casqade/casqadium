#ifndef RENDER_COMMAND_HPP
#define RENDER_COMMAND_HPP

#include <functional>


namespace TimeUtils
{
class Duration;
}

///
/// \brief  Stores and executes rendering instructions
///
class RenderCommand
{
  std::function <void( const TimeUtils::Duration )> mCallback;

public:
  RenderCommand( std::function <void( const TimeUtils::Duration )> callback = {} );
///
/// \brief Performs a rendering operation implemented by derived classes
///
/// \param timeFromLatestTick:  Elapsed time from latest tick.
///                             Used for coordinates interpolation
///
  virtual void draw( const TimeUtils::Duration timeFromLatestTick = {} );
};


#endif
