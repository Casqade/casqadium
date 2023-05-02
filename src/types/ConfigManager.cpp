#include <cqde/types/ConfigManager.hpp>

#include <cqde/assert.hpp>
#include <cqde/file_helpers.hpp>
#include <cqde/json_helpers.hpp>

#include <spdlog/fmt/bundled/format.h>

#include <json/value.h>
#include <json/reader.h>
#include <json/writer.h>

#include <iostream>
#include <fstream>


namespace cqde::types
{

ConfigManager::ConfigManager(
  Path configPath )
  : mConfigPath{configPath}
  , mConfig{std::make_shared <Config> ()}
{}

void
ConfigManager::read() const
{
  CQDE_ASSERT_DEBUG(mConfig != nullptr, return);

  Json::Value config {};

  try
  {
    config = fileParse(mConfigPath);
  }
  catch ( const std::exception& e )
  {
    using fmt::format;
    std::cerr <<
      format("Configuration file read error: {}\n",
             e.what());

    return;
  }

  mConfig->fromJson(config);
}

void
ConfigManager::write() const
{
  CQDE_ASSERT_DEBUG(mConfig != nullptr, return);

  try
  {
    const auto streamFlags = std::ios::out |
                             std::ios::trunc |
                             std::ios::binary;

    auto fileStream = fileOpen(mConfigPath, streamFlags);
    fileStream << Json::writeString(jsonWriter(), mConfig->toJson());
  }
  catch ( const std::exception& e )
  {
    using fmt::format;
    std::cerr <<
      format("Configuration file write error: {}\n",
             e.what());
  }
}

void
ConfigManager::setConfig(
  const std::weak_ptr <Config> config )
{
  CQDE_ASSERT_DEBUG(mConfig != nullptr, return);
  mConfig = config.lock();
}

std::weak_ptr <Config>
ConfigManager::config() const
{
  return mConfig;
}

} // namespace cqde::types
