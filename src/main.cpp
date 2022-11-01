#include <AnotherDayAtHospital.hpp>

#include <spdlog/fmt/bundled/format.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

#include <cqde/types/ConfigManager.hpp>
#include <cqde/util/logger.hpp>

#include <iostream>
#include <filesystem>


int
main( int, char* argv[] )
{
  using log_level = spdlog::level::level_enum;
  using spdlog::sinks::stdout_color_sink_mt;
  using spdlog::sinks::basic_file_sink_mt;
  using path = std::filesystem::path;
  using cqde::types::ConfigManager;

  const auto executableName = path{argv[0]}.stem();

  const auto configFilename = executableName.string() + ".conf";
  const auto logFilename = executableName.string() + ".log";

  const ConfigManager configManager (configFilename);

  std::vector <std::shared_ptr <spdlog::sinks::sink>> sinks {};

  if ( configManager.logLevelCmd() < log_level::off )
  {
    auto stdoutSink = std::make_shared <stdout_color_sink_mt> ();
    stdoutSink->set_level(configManager.logLevelCmd());

    sinks.push_back(stdoutSink);
  }

  if ( configManager.logLevelFile() < log_level::off )
  {
    try
    {
      auto fileSink = std::make_shared <basic_file_sink_mt> (logFilename, true);
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

  AnotherDayAtHospital game(configManager);

  const olc::rcode result = game.Construct(
    configManager.windowWidth(),
    configManager.windowHeight(),
    1, 1,
    configManager.fullscreenEnabled(),
    false, true );

  return result != olc::rcode::OK || game.Start() != olc::rcode::OK;
}
