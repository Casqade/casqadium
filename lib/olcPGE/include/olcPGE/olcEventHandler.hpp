#pragma once

#include <olcPGE/olcPGE_export.hpp>
#include <olcPGE/olcEvent.hpp>

#include <deque>


namespace olc
{
class Event;

class OLCPGE_EXPORT EventHandler
{
  const PixelGameEngine* const mPGE;

  std::deque <olc::Event> mEvents;

  bool acceptKeyHeldEvent( const Event& ) const;
  bool acceptButtonHeldEvent( const Event& ) const;

public:
  EventHandler() = delete;
  EventHandler( const PixelGameEngine* const );

  void update();
  bool pollEvent( olc::Event& event );
};

} // namespace olc
