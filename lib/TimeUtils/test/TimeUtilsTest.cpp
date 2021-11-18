#include <TimeUtils/Duration.hpp>
#include <fstream>


int
main()
{
  std::ofstream logger("testResults.txt", std::ios::out);
  if ( logger.is_open() == false )
    return 0;

  logger << "100ms x 1: " << TimeUtils::averageSleepTime( 0.100'000, 1 ) << "\n";
  logger << "100ms x 10: " << TimeUtils::averageSleepTime( 0.100'000, 10 ) << "\n";
  logger << "100ms x 100: " << TimeUtils::averageSleepTime( 0.100'000, 100 ) << "\n";

  logger << "10ms x 1: " << TimeUtils::averageSleepTime( 0.010'000, 1 ) << "\n";
  logger << "10ms x 10: " << TimeUtils::averageSleepTime( 0.010'000, 10 ) << "\n";
  logger << "10ms x 100: " << TimeUtils::averageSleepTime( 0.010'000, 100 ) << "\n";
  logger << "10ms x 1000: " << TimeUtils::averageSleepTime( 0.010'000, 1000 ) << "\n";

  logger << "1ms x 1: " << TimeUtils::averageSleepTime( 0.001'000, 1 ) << "\n";
  logger << "1ms x 10: " << TimeUtils::averageSleepTime( 0.001'000, 10 ) << "\n";
  logger << "1ms x 100: " << TimeUtils::averageSleepTime( 0.001'000, 100 ) << "\n";
  logger << "1ms x 1000: " << TimeUtils::averageSleepTime( 0.001'000, 1000 ) << "\n";
  logger << "1ms x 10000: " << TimeUtils::averageSleepTime( 0.001'000, 10000 ) << "\n";

  logger << "500us x 1: " << TimeUtils::averageSleepTime( 0.000'500, 1 ) << "\n";
  logger << "500us x 10: " << TimeUtils::averageSleepTime( 0.000'500, 10 ) << "\n";
  logger << "500us x 100: " << TimeUtils::averageSleepTime( 0.000'500, 100 ) << "\n";

  logger << "100us x 1: " << TimeUtils::averageSleepTime( 0.000'100, 1 ) << "\n";
  logger << "100us x 10: " << TimeUtils::averageSleepTime( 0.000'100, 10 ) << "\n";
  logger << "100us x 100: " << TimeUtils::averageSleepTime( 0.000'100, 100 ) << "\n";

  logger << "50us x 1: " << TimeUtils::averageSleepTime( 0.000'050, 1 ) << "\n";
  logger << "50us x 10: " << TimeUtils::averageSleepTime( 0.000'050, 10 ) << "\n";
  logger << "50us x 100: " << TimeUtils::averageSleepTime( 0.000'050, 100 ) << "\n";

  logger << "10us x 1: " << TimeUtils::averageSleepTime( 0.000'010, 1 ) << "\n";
  logger << "10us x 10: " << TimeUtils::averageSleepTime( 0.000'010, 10 ) << "\n";
  logger << "10us x 100: " << TimeUtils::averageSleepTime( 0.000'010, 100 ) << "\n";
  logger << "10us x 1000: " << TimeUtils::averageSleepTime( 0.000'010, 1000 ) << "\n";

  logger << "1us x 1: " << TimeUtils::averageSleepTime( 0.000'001, 1 ) << "\n";
  logger << "1us x 10: " << TimeUtils::averageSleepTime( 0.000'001, 10 ) << "\n";
  logger << "1us x 100: " << TimeUtils::averageSleepTime( 0.000'001, 100 ) << "\n";
  logger << "1us x 1000: " << TimeUtils::averageSleepTime( 0.000'001, 1000 ) << "\n";
  logger << "1us x 10000: " << TimeUtils::averageSleepTime( 0.000'001, 10000 ) << "\n";

  logger.close();

  return 0;
}

