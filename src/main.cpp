#include <AnotherDayAtHospital.hpp>

#include <spdlog/sinks/ansicolor_sink.h>
#include <spdlog/sinks/basic_file_sink.h>

#include <cqde/util/ConfigManager.hpp>
#include <cqde/util/logger.hpp>

int
main( int , char*[] )
{
  const static std::string configFilename = "AnotherDayAtHospital.conf";
  const static std::string logFilename = "AnotherDayAtHospital.log";

  cqde::ConfigManager configManager (configFilename);

  std::vector <std::shared_ptr <spdlog::sinks::sink>> sinks {};

  if ( configManager.logLevelCmd() < spdlog::level::level_enum::off )
  {
    auto cmdSink = std::make_shared <spdlog::sinks::ansicolor_stdout_sink_mt> ();
    cmdSink->set_level(configManager.logLevelCmd());

    sinks.push_back(cmdSink);
  }

  if ( configManager.logLevelFile() < spdlog::level::level_enum::off )
  {
    auto fileSink = std::make_shared <spdlog::sinks::basic_file_sink_mt> (logFilename, true);
    fileSink->set_level(configManager.logLevelFile());

    sinks.push_back(fileSink);
  }

  cqde::loggerInit( configManager.logPattern(), sinks );

  AnotherDayAtHospital game( configManager.tickRate(),
                             configManager.frameRate() );

  const olc::rcode result = game.Construct(
    configManager.windowWidth(),
    configManager.windowHeight(),
    1, 1,
    configManager.fullscreenEnabled(),
    false, true );

  if ( result != olc::rcode::OK )
    return result;

  return game.Start() == olc::rcode::OK;
}
