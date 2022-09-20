#include <AnotherDayAtHospital.hpp>

#include <spdlog/fmt/bundled/format.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

#include <cqde/types/ConfigManager.hpp>
#include <cqde/util/logger.hpp>

#include <iostream>


int
main( int , char*[] )
{
  const std::string configFilename = "AnotherDayAtHospital.conf";
  const std::string logFilename = "AnotherDayAtHospital.log";

  const cqde::types::ConfigManager configManager (configFilename);

  std::vector <std::shared_ptr <spdlog::sinks::sink>> sinks {};

  if ( configManager.logLevelCmd() < spdlog::level::level_enum::off )
  {
    auto stdoutSink = std::make_shared <spdlog::sinks::stdout_color_sink_mt> ();
    stdoutSink->set_level(configManager.logLevelCmd());

    sinks.push_back(stdoutSink);
  }

  if ( configManager.logLevelFile() < spdlog::level::level_enum::off )
  {
    try
    {
      auto fileSink = std::make_shared <spdlog::sinks::basic_file_sink_mt> (logFilename, true);
      fileSink->set_level(configManager.logLevelFile());
      sinks.push_back(fileSink);
    }
    catch ( const std::exception& e )
    {
      using fmt::format;
      std::cerr <<
        format( "Error: Can't create file sink for logger ({}). "
                "Log output to file will be turned off\n",
                e.what());
    }
  }

  cqde::loggerInit( configManager.logPattern(),
                    configManager.logFlushLevel(),
                    sinks );

  AnotherDayAtHospital game( configManager.tickRate(),
                             configManager.frameRate() );

  const olc::rcode result = game.Construct(
    configManager.windowWidth(),
    configManager.windowHeight(),
    1, 1,
    configManager.fullscreenEnabled(),
    false, true );

  return result != olc::rcode::OK || game.Start() != olc::rcode::OK;
}
