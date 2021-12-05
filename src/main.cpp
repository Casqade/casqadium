#include <AnotherDayAtHospital.hpp>

#include <Logger.hpp>


Logger Log;

int
main( int , char*[] )
{
  AnotherDayAtHospital game( 30, 60 );

  return game.Start() == olc::rcode::OK;
}
