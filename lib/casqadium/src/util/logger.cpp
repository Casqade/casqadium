#include <cqde/util/logger.hpp>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/dist_sink.h>
#include <spdlog/async.h>


namespace cqde
{

void
loggerInit(
  const std::string& pattern,
  const std::vector <std::shared_ptr <spdlog::sinks::sink>>& sinks )
{
  auto dist_sink = std::make_shared <spdlog::sinks::dist_sink_mt> (sinks);
  dist_sink->set_level(spdlog::level::trace);

//  spdlog::init_thread_pool(spdlog::details::default_async_q_size, 1 );
//  auto logger = std::make_shared <spdlog::async_logger> ( "cqde", dist_sink,
//                                                          spdlog::thread_pool() );

  auto logger = std::make_shared <spdlog::logger> ( "cqde", dist_sink );
  logger->set_level(spdlog::level::trace);
  logger->set_pattern(pattern);

  spdlog::register_logger(logger);
  spdlog::set_default_logger(logger);

  LOG_INFO("casqadium logger initialized");
}

} // namespace cqde
