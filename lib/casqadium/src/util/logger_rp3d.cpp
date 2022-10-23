#include <cqde/util/logger.hpp>
#include <cqde/util/logger_rp3d.hpp>


namespace cqde
{

void
logger_rp3d::log(
  Level level,
  const std::string& physicsWorldName,
  Category category,
  const std::string& message,
  const char* filename,
  int lineNumber )
{
  using spdlog::level::level_enum;

  level_enum spdlog_level {};

  switch (level)
  {
    case Level::Error:
      spdlog_level = level_enum::err;
      break;

    case Level::Warning:
      spdlog_level = level_enum::warn;
      break;

    case Level::Information:
      spdlog_level = level_enum::trace;
      return;
  }

  spdlog::log({filename, lineNumber, {}},
              spdlog_level,
              message);
}

} // namespace cqde
