#pragma once

#include <spdlog/spdlog.h>

#include <string>
#include <vector>
#include <memory>


namespace cqde
{

void loggerInit( const std::string& pattern,
                 const spdlog::level::level_enum flushLevel,
                 const std::vector <std::shared_ptr <spdlog::sinks::sink>>& );

} // namespace cqde

#define LOG_TRACE(...) ::spdlog::log( \
  spdlog::source_loc{__FILE__, __LINE__, SPDLOG_FUNCTION}, \
  spdlog::level::trace, \
  __VA_ARGS__)

#define LOG_DEBUG(...) ::spdlog::log( \
  spdlog::source_loc{__FILE__, __LINE__, SPDLOG_FUNCTION}, \
  spdlog::level::debug, \
  __VA_ARGS__)

#define LOG_INFO(...) ::spdlog::log( \
  spdlog::source_loc{__FILE__, __LINE__, SPDLOG_FUNCTION}, \
  spdlog::level::info, \
  __VA_ARGS__)

#define LOG_WARN(...) ::spdlog::log( \
  spdlog::source_loc{__FILE__, __LINE__, SPDLOG_FUNCTION}, \
  spdlog::level::warn, \
  __VA_ARGS__)

#define LOG_ERROR(...) ::spdlog::log( \
  spdlog::source_loc{__FILE__, __LINE__, SPDLOG_FUNCTION}, \
  spdlog::level::err, \
  __VA_ARGS__)

#define LOG_CRITICAL(...) ::spdlog::log( \
  spdlog::source_loc{__FILE__, __LINE__, SPDLOG_FUNCTION}, \
  spdlog::level::critical, \
  __VA_ARGS__)

// assert in debug build; handle condition safely in release
#define LOG_ASSERT_DEBUG(cond, action)\
  if (!(cond)) {assert(cond); action;}
