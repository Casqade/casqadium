#pragma once

#include <cqde/types/Config.hpp>

#include <filesystem>


namespace cqde::types
{

class ConfigManager
{
  using Path = std::filesystem::path;

  Path mConfigPath {};

  std::shared_ptr <Config> mConfig {};

public:
  ConfigManager( Path configPath = {} );
  virtual ~ConfigManager() = default;

  void read() const;
  void write() const;

  void setPath( const Path& );
  Path& path() const;

  void setConfig( const std::weak_ptr <Config> );
  std::weak_ptr <Config> config() const;
};

} // namespace cqde::types
