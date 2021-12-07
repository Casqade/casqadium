#ifndef OLC_PGE_EVENT_HANDLER
#define OLC_PGE_EVENT_HANDLER

#include <olcPGE/olcEvent.hpp>
#include <deque>


namespace olc
{
class Event;

class EventHandler
{
  const PixelGameEngine* const mPGE;

  std::deque <olc::Event> mEvents;

public:
  EventHandler() = delete;
  EventHandler( const PixelGameEngine* const );

  void update();
  bool pollEvent( olc::Event& event );
};

} // namespace olc

#endif
