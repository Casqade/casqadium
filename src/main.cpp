#include <AnotherDayAtHospital.hpp>

#include <Logger.hpp>


Logger Log;

int
main( int , char*[] )
{
  AnotherDayAtHospital game( 30, 60 );

  const olc::rcode result = game.Construct( 800, 600, 1, 1 );
  if ( result != olc::rcode::OK )
    return result;

  return game.Start() == olc::rcode::OK;
}
