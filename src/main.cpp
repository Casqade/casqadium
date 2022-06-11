#include <AnotherDayAtHospital.hpp>

#include <spdlog/sinks/stdout_color_sinks.h>
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

  auto stdErrSink = std::make_shared <spdlog::sinks::stderr_color_sink_mt> ();
  stdErrSink->set_level(spdlog::level::level_enum::err);

  sinks.push_back(stdErrSink);

  if ( configManager.logLevelCmd() < spdlog::level::level_enum::off )
  {
    auto stdoutSink = std::make_shared <spdlog::sinks::stdout_color_sink_mt> ();
    stdoutSink->set_level(configManager.logLevelCmd());

    sinks.push_back(stdoutSink);
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
