#include <AnotherDayAtHospital.hpp>



int
main( int , char*[] )
{
  AnotherDayAtHospital game( 30, 60 );

  const olc::rcode result = game.Construct( 800, 600, 1, 1,
                                            false, false, true );
  if ( result != olc::rcode::OK )
    return result;

  return game.Start() == olc::rcode::OK;
}
